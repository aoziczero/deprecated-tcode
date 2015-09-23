#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/select.hpp>
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
#include <sys/select.h>
#include <unistd.h>

namespace tcode { namespace io {

    struct select::_descriptor{
        int fd;
        tcode::slist::queue< tcode::operation > op_queue[tcode::io::ev_max];
        std::atomic<int> refcount;

        void complete( select* ep ,  int ev );

        _descriptor( select* ep , int fd  );

        void add_ref( void );
        void release( select* ep );
    };

    select::_descriptor::_descriptor( select* ep , int fd ) {
        refcount.store(1);
        ep->_engine.active_inc();
        this->fd = fd;
    }

    void select::_descriptor::add_ref( void ) {
        refcount.fetch_add(1);
    }

    void select::_descriptor::release( select* ep ){
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

    void select::_descriptor::complete( select* ep , int events ) {
        while ( !op_queue[events].empty() ) {
            io::operation* op = op_queue[events].front<io::operation>();
            if ( op->post_proc(ep,this) ) {
                op_queue[events].pop_front();
                (*op)();
            } else {
                break;
            }
        }
    }

    select::select( engine& en )
        :  _engine( en )
    {
        _desc.push_back(nullptr);
    }

    select::~select( void ){
    }

    int select::run( const tcode::timespan& ts ){
        std::vector< std::pair< int , select::descriptor > > rdvec;
        std::vector< std::pair< int , select::descriptor > > wrvec;

        fd_set rdfds , wrfds;
        FD_ZERO( &rdfds );
        FD_ZERO( &wrfds );
        FD_SET( _wake_up.rd_pipe() , &rdfds );
        rdvec.push_back( std::make_pair( _wake_up.rd_pipe() , nullptr ));
        int maxfd = _wake_up.rd_pipe();
        for ( int i = 1 ; i < _desc.size() ; ++i ) {
            if (!_desc[i]->op_queue[ev_read].empty() ) {
                rdvec.push_back( std::make_pair( _desc[i]->fd , _desc[i] ));
                FD_SET( _desc[i]->fd , &rdfds );
                if ( maxfd < _desc[i]->fd )
                    maxfd = _desc[i]->fd;
            }
            if (!_desc[i]->op_queue[ev_write].empty()){
                wrvec.push_back( std::make_pair( _desc[i]->fd , _desc[i] ) );
                FD_SET( _desc[i]->fd , &wrfds );
                if ( maxfd < _desc[i]->fd )
                    maxfd = _desc[i]->fd;
            }
        }

        struct timeval tv;
        tv.tv_sec = static_cast< long >( ts.total_microseconds() / ( 1000 * 1000 ));
        tv.tv_usec = ts.total_microseconds() % ( 1000 * 1000 );
        int ret = ::select( maxfd + 1 , &rdfds , &wrfds , nullptr  , &tv );
        if ( ret <= 0  ) {
            return 0; 
        }
        int run = 0;
        bool execute_op = false;
        for ( std::size_t i  = 0 ; i < rdvec.size() ;  ++i ) {
            if ( FD_ISSET( rdvec[i].first, &rdfds ) ) {
                if ( rdvec[i].second != nullptr ) {
                    rdvec[i].second->complete( this , ev_read );
                    ++run;
                } else {
                    char pipe_r[256];
                    ::read( _wake_up.rd_pipe() , pipe_r , 256 );
                    execute_op = true;
                }
            }
        }
        for ( std::size_t i  = 0 ; i < wrvec.size() ;  ++i ) {
            if ( FD_ISSET( wrvec[i].first , &wrfds ) ) {
                if ( wrvec[i].second != nullptr ) {
                    wrvec[i].second->complete( this , ev_write );
                    ++run;
                } 
            }
        }
        if ( execute_op ){
            tcode::slist::queue< tcode::operation > ops;
            do {
                tcode::threading::spinlock::guard guard(_lock);
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
    
    void select::wake_up( void ){
        char ch=0;
        ::write( _wake_up.wr_pipe() , &ch , 1 );
    }

    bool select::bind( const descriptor& d ) {
        execute( tcode::operation::wrap([this,d]{
                        _desc.push_back(d);
                    }));
        return true;
    }

    void select::unbind( descriptor& d ) {
        do { 
            tcode::threading::spinlock::guard guard(_lock);
            if ( d == nullptr ) 
                return;
            descriptor desc = nullptr;
            std::swap( desc , d );
            op_add( tcode::operation::wrap( 
                    [this,desc] {
                        std::size_t i = 0;
                        for ( ; i < _desc.size() ; ++i ){
                            if ( _desc[i] == desc )
                                break;
                        }
                        if ( i != _desc.size()) {
                            if ( i < _desc.size() -1 ) {
                                std::swap( _desc[i] , _desc[_desc.size() -1 ] );
                            }
                            _desc.pop_back();
                        }
                        if ( desc->fd != -1 ) {
                            ::close( desc->fd );
                            desc->fd = -1;
                        }
                        desc->release( this );
                    })
            );
        }while(0);
        wake_up();
    }

    void select::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            op_add(op);
        } while(0);
        wake_up();
    }

    void select::connect( 
            select::descriptor& desc 
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
                desc = new select::_descriptor(this , fd );
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

    void select::write( select::descriptor desc 
            , ip::tcp::operation_write_base* op)
    {
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , ev_write );
                desc->release(this);
            }));
    }

    void select::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , ev_read );
                desc->release(this);
            }));

    }
    
    bool select::listen( descriptor& desc 
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
                desc = new select::_descriptor(this,fd);
                if ( bind( desc ))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void select::accept( descriptor desc
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
                desc->complete( this , ev_read );
                desc->release(this);
            }));
    }

    bool select::open( descriptor& desc
                , int af , int type , int proto )
    {
        int fd = socket( af , type , proto );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            desc = new select::_descriptor( this , fd );
            if ( bind(desc))
                return true;
            delete desc;
            desc = nullptr;
            ::close(fd);
        }
        return false;
    }
    bool select::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        int fd = socket( addr.family() , SOCK_DGRAM , IPPROTO_UDP );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0 ){
                desc = new select::_descriptor( this , fd );
                if ( bind(desc))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void select::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
            int fd = socket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP);
            if ( fd != -1 ){
                desc = new select::_descriptor( this , fd );
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
                desc->complete( this , ev_write );
                desc->release(this);
            }));
    }
    
    void select::read( descriptor desc
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
                desc->complete( this , ev_read);
                desc->release(this);
            }));
    }


    void select::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.active_inc();
    }

    void select::op_run( tcode::operation* op ){
        _engine.active_dec();
        (*op)();
    }

    int select::writev( descriptor desc 
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

    int select::readv( descriptor desc 
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

    int select::accept( descriptor listen 
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

                accepted = new select::_descriptor( this , fd );
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

    int select::read( descriptor desc 
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

    int select::write( descriptor desc 
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

    int select::native_descriptor( descriptor d ) {
        return d->fd;
    }
}}

