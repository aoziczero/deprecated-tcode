#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/poll.hpp>
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
#include <sys/poll.h>
#include <unistd.h>

namespace tcode { namespace io {

    struct poll::_descriptor{
        int fd;
        tcode::slist::queue< tcode::operation > op_queue[tcode::io::ev_max];
        std::atomic<int> refcount;
        int pollidx;

        void complete( poll* ep ,  int ev );

        _descriptor( poll* ep , int fd  );

        void add_ref( void );
        void release( poll* ep );
    };

    poll::_descriptor::_descriptor( poll* ep , int fd ) {
        refcount.store(1);
        ep->_engine.active_inc();
        this->fd = fd;
    }

    void poll::_descriptor::add_ref( void ) {
        refcount.fetch_add(1);
    }

    void poll::_descriptor::release( poll* ep ){
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

    void poll::_descriptor::complete( poll* ep , int events ) {
        static const int pollev[] = {
            POLLIN , POLLOUT , POLLPRI 
        };
        for ( int i = 0 ; i < tcode::io::ev_max ; ++i ) {
            if ( events & pollev[i] ){
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
        
        ep->_events[pollidx].events = op_queue[tcode::io::ev_read].empty() ? 0 : POLLIN;
        ep->_events[pollidx].events |= op_queue[tcode::io::ev_write].empty() ? 0 : POLLOUT;
    }

    poll::poll( engine& en )
        :  _engine( en )
    {
        struct pollfd pe;
        pe.fd = _wake_up.rd_pipe();
        pe.events = POLLIN;
        _events.push_back( pe );
        _desc.push_back(nullptr);
    }

    poll::~poll( void ){
    }

    int poll::run( const tcode::timespan& ts ){
        int nofd  = ::poll( &_events[0] , _events.size() 
           , static_cast<int>(ts.total_milliseconds()));
        if ( nofd <= 0  ) {
            return 0; 
        }
        int run = nofd;
        bool execute_op = false;
        for ( std::size_t i  = 0 ; i < _events.size() ;  ++i ) {
            if ( _events[i].revents & ( POLLIN | POLLOUT  ) ) {
                if ( _desc[i] != nullptr )
                    _desc[i]->complete( this , _events[i].revents );
                else {
                    char pipe_r[256];
                    ::read( _wake_up.rd_pipe() , pipe_r , 256 );
                    execute_op = true;
                    --run;
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
    
    void poll::wake_up( void ){
        char ch=0;
        ::write( _wake_up.wr_pipe() , &ch , 1 );
    }

    bool poll::bind( const descriptor& d ) {
        execute( tcode::operation::wrap([this,d]{
                    struct pollfd e;
                    e.fd = d->fd;
                    e.events = POLLIN | POLLOUT;
                    _events.push_back( e );
                    _desc.push_back(d);
                    d->pollidx = _events.size() - 1 ;
                    }));
        return true;
    }

    void poll::unbind( descriptor& d ) {
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
                                std::swap( _events[i] , _events[_events.size() -1 ] );
                                _desc[i]->pollidx = i;
                            }
                            _desc.pop_back();
                            _events.pop_back();
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

    void poll::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            op_add(op);
        } while(0);
        wake_up();
    }

    void poll::connect( 
            poll::descriptor& desc 
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
                desc = new poll::_descriptor(this , fd );
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

    void poll::write( poll::descriptor desc 
            , ip::tcp::operation_write_base* op)
    {
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , POLLOUT );
                desc->release(this);
            }));
    }

    void poll::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , POLLIN );
                desc->release(this);
            }));

    }
    
    bool poll::listen( descriptor& desc 
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
                desc = new poll::_descriptor(this,fd);
                if ( bind( desc ))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void poll::accept( descriptor desc
            , int
            , ip::tcp::operation_accept_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_accept].push_back(op);
                desc->complete( this , POLLIN );
                desc->release(this);
            }));
    }
    
    bool poll::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        int fd = socket( addr.family() , SOCK_DGRAM , IPPROTO_UDP );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0 ){
                desc = new poll::_descriptor( this , fd );
                if ( bind(desc))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void poll::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
            int fd = socket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP);
            if ( fd != -1 ){
                desc = new poll::_descriptor( this , fd );
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
                desc->complete( this , POLLOUT );
                desc->release(this);
            }));
    }
    
    void poll::read( descriptor desc
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
                desc->complete( this , POLLIN);
                desc->release(this);
            }));
    }


    void poll::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.active_inc();
    }

    void poll::op_run( tcode::operation* op ){
        _engine.active_dec();
        (*op)();
    }

    int poll::writev( descriptor desc 
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

    int poll::readv( descriptor desc 
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

    int poll::accept( descriptor listen 
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

                accepted = new poll::_descriptor( this , fd );
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

    int poll::read( descriptor desc 
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

    int poll::write( descriptor desc 
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

    int poll::native_descriptor( descriptor d ) {
        return d->fd;
    }
}}
/*
#include "stdafx.h"
#include <tcode/io/io.hpp>
#include <tcode/io/poll.hpp>
#include <tcode/io/option.hpp>
#include <tuple>
#include <unistd.h>

namespace tcode { namespace io {

    poll::poll( void ) 
        : _pipe_handler( [this]( int ev ) {
                    wake_up_handler( ev );
                })
    {
        tcode::io::option::nonblock nb;
        nb.set_option( _pipe.rd_pipe());
        bind( _pipe.rd_pipe() , EV_READ , &_pipe_handler );
    }

    poll::~poll( void ) {
    }

    bool poll::bind( int fd , int ev , tcode::function< void (int) >* handler ){
        int pollev = 0;
        if ( ev & EV_READ) pollev = POLLIN;
        if ( ev & EV_WRITE) pollev |= POLLOUT;
        for ( std::size_t i = 0 ; i < _fds.size() ; ++i ) {
            if ( _fds[i].fd == fd ) {
                _fds[i].events = pollev;
               _handler[i] = handler;
                return true;
            }
        }
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = pollev; 
        _fds.push_back( pfd );
        _handler.push_back( handler );
        return true;
    }


    void poll::unbind( int fd ) {
        std::size_t i = 0;
        for (  ; i < _fds.size(); ++i ){
            if ( _fds[i].fd == fd ) {
                break;
            }        
        }
        if ( i == _fds.size() )
            return;
        if ( i != _fds.size() -1 ){
            _fds[i] = _fds[ _fds.size() - 1];
            _handler[i] = _handler[ _handler.size() -1];
        }
        _fds.pop_back();
        _handler.pop_back();
    }
    
    
    int poll::run( const tcode::timespan& ts ){
        if ( _fds.empty() )
            return 0;
        int ret =  ::poll( &_fds[0] , _fds.size() , ts.total_milliseconds());
        if ( ret > 0 ) {
            std::vector< std::tuple< int , tcode::function< void ( int ) >* > > events;
            for ( std::size_t i = 0; i < _fds.size() ; ++i ){
                if ( _fds[i].revents & ( POLLIN | POLLOUT ) ) {
                    int ev = 0;
                    if ( _fds[i].revents & POLLIN )
                        ev = EV_READ;
                    if ( _fds[i].revents & POLLOUT )
                        ev |= EV_WRITE;
                    events.push_back( std::make_tuple( ev , _handler[i] ));
                }
            }
            for ( std::size_t i = 0 ; i < events.size() ; ++i ) {
                (*std::get<1>(events[i]))( std::get<0>( events[i]));
            }
        }
        return ret;
    }
    
    void poll::wake_up( void ){
        char ch=0;
        write( _pipe.wr_pipe() , &ch , 1 );
    }
    void poll::wake_up_handler( int ev ){
        char ch;
        read( _pipe.rd_pipe() , &ch , 1 );
    }

}} */
