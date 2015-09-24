#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/rts.hpp>
#include <tcode/io/engine.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/ip/option.hpp>
#include <tcode/io/ip/address.hpp>
#include <tcode/io/ip/tcp/operation_connect.hpp>
#include <tcode/io/ip/tcp/operation_write.hpp>
#include <tcode/io/ip/tcp/operation_read.hpp>
#include <tcode/io/ip/tcp/operation_accept.hpp>
#include <tcode/io/ip/udp/operation_write.hpp>
#include <tcode/io/ip/udp/operation_read.hpp>
#include <sys/rts.h>
#include <unistd.h>

namespace tcode { namespace io {

    namespace {
        const int rts_max_event = 256;
    }

    struct rts::_descriptor{
        int fd;
        tcode::slist::queue< tcode::operation > op_queue[tcode::io::ev_max];
        std::atomic<int> refcount;

        void complete( rts* ep ,  int ev );

        _descriptor( rts* ep , int fd  );

        void add_ref( void );
        void release( rts* ep );
    };

    rts::_descriptor::_descriptor( rts* ep , int fd ) {
        refcount.store(1);
        ep->_engine.active_inc();
        this->fd = fd;
    }

    void rts::_descriptor::add_ref( void ) {
        refcount.fetch_add(1);
    }

    void rts::_descriptor::release( rts* ep ){
        if ( refcount.fetch_sub(1) != 1 ) {
            return; 
        }
        ep->_engine.active_dec();
        
        tcode::slist::queue< tcode::operation > ops;
        for ( int i = 0 ;i < tcode::io::ev_max ; ++i ) {
            while ( !op_queue[i].empty()){
                io::operation* op = op_queue[i].front< io::operation >();
                op_queue[i].pop_front();
                op->error() = tcode::error_aborted;
                ops.push_back(op);
            }
        }

        if ( !ops.empty() ) {
            do {
                tcode::threading::spinlock::guard guard(ep->_lock);
                while( !ops.empty()){
                    tcode::operation* op = ops.front();
                    ops.pop_front();
                    ep->op_add(op);
                }
            }while(0);
            ep->wake_up();
        }
        delete this;
    }

    void rts::_descriptor::complete( rts* ep , int events ) {
        static const int rtsev[] = {
            EPOLLIN , EPOLLOUT , EPOLLPRI 
        };
        for ( int i = 0 ; i < tcode::io::ev_max ; ++i ) {
            if ( events & rtsev[i] ){
                while ( !op_queue[i].empty() ) {
                    io::operation* op = op_queue[i].front<io::operation>();
                    if ( op->post_proc(ep,this) ) {
                        op_queue[i].pop_front();
                        (*op)();
                    } else {
                        break;
                    }
                }
            }
        }
    }

    rts::rts( engine& en )
        : _handle( rts_create(rts_max_event))
        , _engine( en )
    {
        struct rts_event e;
        e.events = EPOLLIN ;
        e.data.ptr = nullptr;
        rts_ctl( _handle , EPOLL_CTL_ADD , _wake_up.rd_pipe() , &e );
    }

    rts::~rts( void ){
        ::close( _handle );
    }

    int rts::run( const tcode::timespan& ts ){
        struct rts_event events[rts_max_event];
        int nofd  = rts_wait( _handle 
            , events
            , rts_max_event
            , static_cast<int>(ts.total_milliseconds()));
        if ( nofd <= 0  ) {
            return 0; 
        }
        int run = nofd;
        bool execute_op = false;
        for ( int i = 0 ; i < nofd; ++i ){
            rts::descriptor desc =
                static_cast< rts::descriptor >( events[i].data.ptr );
            if ( desc ) {
                desc->complete( this , events[i].events );
            } else {
                char pipe_r[256];
                ::read( _wake_up.rd_pipe() , pipe_r , 256 );
                execute_op = true;
                --run;
            }
        }
        if ( execute_op ){
            tcode::slist::queue< tcode::operation > ops;
            do {
                tcode::threading::spinlock::guard guard(_lock);
                ops = std::move(_op_queue );
            }while(0);
            while( !ops.empty() ){
                tcode::operation* op = ops.front();
                ops.pop_front();
                op_run( op );
                ++run;
            }
        }
        return run;
    }
    
    void rts::wake_up( void ){
        char ch = 0;
        ::write( _wake_up.wr_pipe() , &ch , 1 );
    }

    bool rts::bind( const descriptor& d ) {
        struct rts_event e;
        e.events = EPOLLIN | EPOLLOUT | EPOLLET;
        e.data.ptr = d;
        return rts_ctl( _handle , EPOLL_CTL_ADD , d->fd , &e ) == 0;
    }

    void rts::unbind( descriptor& d ) {
        do { 
            tcode::threading::spinlock::guard guard(_lock);
            if ( d == nullptr ){
                return;
            }
            descriptor desc = d;
            d = nullptr;
            op_add( tcode::operation::wrap( 
                    [this,desc] {
                        if ( desc->fd != -1 ) {
                            rts_ctl( _handle , EPOLL_CTL_DEL , desc->fd , nullptr );
                            ::close( desc->fd );
                            desc->fd = -1;
                        }
                        desc->release( this );
                    })
            );
        }while(0);
        wake_up();
    }

    void rts::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            op_add(op);
        } while(0);
        wake_up();
    }

    void rts::connect( 
            rts::descriptor& desc 
            , ip::tcp::operation_connect_base* op )
    {
        int fd = socket( op->address().family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd == -1 ) {
            op->error() = tcode::last_error(); 
        } else {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option( fd );
            int r;
            do {
                r = ::connect( fd , op->address().sockaddr() , op->address().sockaddr_length());
            }while((r == -1) && (errno == EINTR));
            if ( (r == 0) || (errno == EINPROGRESS )){
                desc = new rts::_descriptor(this , fd );
                desc->op_queue[tcode::io::ev_connect].push_back( op );
                if ( bind( desc ) ){
                    return;
                }
                op->error() = tcode::last_error();
                desc->op_queue[tcode::io::ev_connect].pop_front();
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
            op->error() = tcode::last_error();
        }
        execute(op);
    }

    void rts::write( rts::descriptor desc 
            , ip::tcp::operation_write_base* op)
    {
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , EPOLLOUT );
                desc->release(this);
            }));
    }

    void rts::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , EPOLLIN );
                desc->release(this);
            }));

    }
    
    bool rts::listen( descriptor& desc 
                , const ip::address& addr )
    {
        int fd = socket( addr.family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd != -1 ){
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ((::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0)
                    && (::listen(fd, SOMAXCONN ) == 0 )) {
                desc = new rts::_descriptor(this,fd);
                if ( bind( desc ))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void rts::accept( descriptor desc
            , int
            , ip::tcp::operation_accept_base* op ){
        if ( desc == nullptr ){
            //todo
            return;
        }

        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_accept].push_back(op);
                desc->complete( this , EPOLLIN );
                desc->release(this);
            }));
    }
    
    bool rts::open( descriptor& desc
                , int af , int type , int proto )
    {
        int fd = socket( af , type , proto );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            desc = new rts::_descriptor( this , fd );
            if ( bind(desc))
                return true;
            delete desc;
            desc = nullptr;
            ::close(fd);
        }
        return false;
    }
        
    
    bool rts::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        int fd = socket( addr.family() , SOCK_DGRAM , IPPROTO_UDP );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0 ){
                desc = new rts::_descriptor( this , fd );
                if ( bind(desc))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void rts::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
            int fd = socket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP);
            if ( fd != -1 ){
                desc = new rts::_descriptor( this , fd );
                if ( !bind( desc )){
                    op->error() = tcode::last_error();
                    delete desc;
                    desc = nullptr;
                    ::close(fd);
                }
            }else{
                op->error() = tcode::last_error();
            }
            if ( desc == nullptr ){
                return execute(op);
            }
        }
        
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , EPOLLOUT );
                desc->release(this);
            }));
    }
    
    void rts::read( descriptor desc
            , ip::udp::operation_read_base* op )
    {
        if ( desc == nullptr ) {
            op->error() = tcode::error_invalid; 
            return execute(op);
        }
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , EPOLLIN);
                desc->release(this);
            }));
    }


    void rts::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.active_inc();
    }

    void rts::op_run( tcode::operation* op ){
        _engine.active_dec();
        (*op)();
    }

    int rts::writev( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::writev( desc->fd , buf , cnt );
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success;
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int rts::readv( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::readv( desc->fd , buf , cnt );
            }while((r == -1) && (errno == EINTR));
            if ( r > 0 ) return r;
            if ( r == 0 ){
                ec = tcode::error_disconnected;
            } else {
                if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                    ec = tcode::error_success;
                else
                    ec = tcode::last_error();
            }
        }
        return -1;
    }

    int rts::accept( descriptor listen 
            , descriptor& accepted 
            , ip::address& addr
            , std::error_code& ec )
    {
        if ( listen->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int fd = -1;
            do {
                fd = ::accept( listen->fd
                        , addr.sockaddr() 
                        , addr.sockaddr_length_ptr());
            }while( ( fd == -1 ) && ( errno == EINTR ));
            if ( fd != -1 ) {
                tcode::io::ip::option::non_blocking nb;
                nb.set_option( fd );

                accepted = new rts::_descriptor( this , fd );
                bind( accepted );
                return 0;
            }
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success; 
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int rts::read( descriptor desc 
            , tcode::io::buffer& buf 
            , tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::recvfrom( desc->fd , buf.buf() , buf.len(),0
                        , addr.sockaddr() , addr.sockaddr_length_ptr());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success; 
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int rts::write( descriptor desc 
            , const tcode::io::buffer& buf 
            , const tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = tcode::error_invalid;
        } else {
            int r = 0;
            do {
                r = ::sendto( desc->fd , buf.buf() , buf.len() , 0
                        , addr.sockaddr() , addr.sockaddr_length());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = tcode::error_success; 
            else
                ec = tcode::last_error();
        }
        return -1;
    }
    
    int rts::native_descriptor( descriptor desc ) {
        return desc->fd;
    }
}}
