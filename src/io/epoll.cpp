#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/epoll.hpp>
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
#include <sys/epoll.h>
#include <unistd.h>

namespace tcode { namespace io {

    namespace {
        const int epoll_max_event = 256;
    }

    struct epoll::_descriptor{
        int fd;
        tcode::slist::queue< tcode::operation > op_queue[tcode::io::ev_max];
        std::atomic<int> refcount;

        void complete( epoll* ep ,  int ev );

        _descriptor( epoll* ep , int fd  );

        void add_ref( void );
        void release( epoll* ep );
    };

    epoll::_descriptor::_descriptor( epoll* ep , int fd ) {
        refcount.store(1);
        ep->_engine.active().inc();
        this->fd = fd;
    }

    void epoll::_descriptor::add_ref( void ) {
        refcount.fetch_add(1);
    }

    void epoll::_descriptor::release( epoll* ep ){
        if ( refcount.fetch_sub(1) != 1 ) {
            return; 
        }
        ep->_engine.active().dec();
        
        tcode::slist::queue< tcode::operation > ops;
        for ( int i = 0 ;i < tcode::io::ev_max ; ++i ) {
            while ( !op_queue[i].empty()){
                tcode::io::operation* op = static_cast< io::operation* >(
                        op_queue[i].front());
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

    void epoll::_descriptor::complete( epoll* ep , int events ) {
        int epollev[] = { EPOLLIN , EPOLLOUT , EPOLLPRI };
        for ( int i = 0 ; i < tcode::io::ev_max ; ++i ) {
            if ( events & epollev[i] ){
                while ( !op_queue[i].empty() ) {
                    io::operation* op = static_cast< io::operation* >( 
                       op_queue[i].front());
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

    epoll::epoll( engine& en )
        : _handle( epoll_create(epoll_max_event))
        , _engine( en )
    {
        struct epoll_event e;
        e.events = EPOLLOUT | EPOLLONESHOT ;
        e.data.ptr = nullptr;
        epoll_ctl( _handle , EPOLL_CTL_ADD , _wake_up.wr_pipe() , &e );
    }

    epoll::~epoll( void ){
        ::close( _handle );
    }

    int epoll::run( const tcode::timespan& ts ){
        struct epoll_event events[epoll_max_event];
        int nofd  = epoll_wait( _handle 
            , events
            , epoll_max_event
            , static_cast<int>(ts.total_milliseconds()));
        if ( nofd <= 0  ) {
            return 0; 
        }
        int run = nofd;
        bool execute_op = false;
        for ( int i = 0 ; i < nofd; ++i ){
            epoll::descriptor desc =
                static_cast< epoll::descriptor >( events[i].data.ptr );
            if ( desc ) {
                desc->complete( this , events[i].events );
            } else {
                execute_op = true;
                --run;
            }
        }
        if ( execute_op ){
            tcode::slist::queue< tcode::operation > ops;
            do {
                tcode::threading::spinlock::guard guard(_lock);
                if ( _op_queue.empty() )
                    return run;
                ops = std::move( _op_queue );
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
    
    void epoll::wake_up( void ){
        struct epoll_event e;
        e.events = EPOLLOUT | EPOLLONESHOT;
        e.data.ptr = nullptr;
        epoll_ctl( _handle , EPOLL_CTL_MOD , _wake_up.wr_pipe() , &e );
    }

    bool epoll::bind( const descriptor& d ) {
        struct epoll_event e;
        e.events = EPOLLIN | EPOLLOUT | EPOLLET;
        e.data.ptr = d;
        return epoll_ctl( _handle , EPOLL_CTL_ADD , d->fd , &e ) == 0;
    }

    void epoll::unbind( descriptor& d ) {
        do { 
            tcode::threading::spinlock::guard guard(_lock);
            if ( d == nullptr ) 
                return;
            descriptor desc = nullptr;
            std::swap( desc , d );
            tcode::operation* op = 
                tcode::operation::wrap( 
                        [this,desc] {
                            if ( desc->fd != -1 ) {
                                epoll_ctl( _handle , EPOLL_CTL_DEL , desc->fd , nullptr );
                                ::close( desc->fd );
                                desc->fd = -1;
                            }
                            desc->release( this );
                        });
            op_add(op);
        }while(0);
        wake_up();
    }

    void epoll::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            op_add(op);
        } while(0);
        wake_up();
    }

    void epoll::connect( 
            epoll::descriptor& desc 
            , ip::tcp::operation_connect_base* op )
    {
        int fd = socket( op->address().family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd == -1 ) {
            op->error() = tcode::last_error(); 
            execute( op );
            return;
        }
        
        tcode::io::ip::option::non_blocking nb;
        nb.set_option( fd );

        int r;
        do {
            r = ::connect( fd , op->address().sockaddr() , op->address().sockaddr_length());
        }while((r == -1) && (errno == EINTR));
        if ( (r == 0 ) || (errno == EINPROGRESS )){
            desc = new epoll::_descriptor(this , fd );
            desc->op_queue[tcode::io::ev_connect].push_back( op );
            bind( desc );
        } else {
            ::close(fd);
            op->error() = tcode::last_error();
            execute(op);
        }
    }

    void epoll::write( epoll::descriptor desc 
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

    void epoll::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , EPOLLIN );
                desc->release(this);
            }));

    }
    
    bool epoll::listen( descriptor& desc 
                , const ip::address& addr )
    {
        int fd = socket( addr.family() , SOCK_STREAM , IPPROTO_TCP );
        if ( fd == -1 )
            return false;
        tcode::io::ip::option::non_blocking nb;
        nb.set_option(fd);
        tcode::io::ip::option::reuse_address reuse( true );
        reuse.set_option( fd );
        if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) < 0 ){
            ::close(fd);
            return false;
        }
        if ( ::listen(fd, SOMAXCONN ) < 0 ) {
            ::close(fd);
            return false;
        }
        desc = new epoll::_descriptor(this,fd);
        bind( desc );
        return true;
    }

    void epoll::accept( descriptor desc
            , ip::tcp::operation_accept_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_accept].push_back(op);
                desc->complete( this , EPOLLIN );
                desc->release(this);
            }));
    }
    
    bool epoll::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        int fd = socket( addr.family() , SOCK_DGRAM , IPPROTO_UDP );
        if ( fd == -1 )
            return false;
        tcode::io::ip::option::non_blocking nb;
        nb.set_option(fd);
        tcode::io::ip::option::reuse_address reuse( true );
        reuse.set_option( fd );
        if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) < 0 ){
            ::close(fd);
            return false;
        }
        desc = new epoll::_descriptor( this , fd );
        bind( desc );
        return true;
    }

    void epoll::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
            int fd = socket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP);
            if ( fd == -1 ){
                op->error() = tcode::last_error();
                execute( op );
                return;
            }
            desc = new epoll::_descriptor( this , fd );
            bind( desc );
        }
        
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , EPOLLOUT );
                desc->release(this);
            }));
    }
    
    void epoll::read( descriptor desc
            , ip::udp::operation_read_base* op )
    {
        if ( desc == nullptr ) {
            op->error() = std::error_code( EBADF , std::generic_category());
            execute(op);
            return;
        }
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , EPOLLIN);
                desc->release(this);
            }));
    }


    void epoll::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.active().inc();
    }

    void epoll::op_run( tcode::operation* op ){
        _engine.active().dec();
        (*op)();
    }

    int epoll::writev( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int r = 0;
            do {
                r = ::writev( desc->fd , buf , cnt );
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code();
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;
    }

    int epoll::readv( descriptor desc 
            , tcode::io::buffer* buf , int cnt
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
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
                    ec = std::error_code();
                else
                    ec = std::error_code( errno , std::generic_category());
            }
        }
        return -1;
    }

    int epoll::accept( descriptor listen 
            , descriptor& accepted 
            , ip::address& addr
            , std::error_code& ec )
    {
        if ( listen->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int fd = -1;
            do {
                fd = ::accept( listen->fd
                        , addr.sockaddr() 
                        , addr.sockaddr_length_ptr());
            }while( ( fd == -1 ) && ( errno == EINTR ));
            if ( fd != -1 ) {
                accepted = new epoll::_descriptor( this , fd );
                bind( accepted );
                return 0;
            }
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code(); 
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;
    }

    int epoll::read( descriptor desc 
            , tcode::io::buffer& buf 
            , tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int r = 0;
            do {
                r = ::recvfrom( desc->fd , buf.buf() , buf.len(),0
                        , addr.sockaddr() , addr.sockaddr_length_ptr());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code(); 
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;
    }

    int epoll::write( descriptor desc 
            , tcode::io::buffer& buf 
            , tcode::io::ip::address& addr 
            , std::error_code& ec )
    {
        if ( desc->fd == -1 ) {
            ec = std::error_code( EBADF , std::generic_category() );
        } else {
            int r = 0;
            do {
                r = ::sendto( desc->fd , buf.buf() , buf.len() , 0
                        , addr.sockaddr() , addr.sockaddr_length());
            }while((r == -1) && (errno == EINTR));
            if ( r >= 0 ) return r;
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code(); 
            else
                ec = std::error_code( errno , std::generic_category());
        }
        return -1;

        return 0;
    }
}}
