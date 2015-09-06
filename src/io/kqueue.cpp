#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/kqueue.hpp>
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
#if defined( TCODE_APPLE )
#include <sys/event.h>
#endif
#include <unistd.h>

namespace tcode { namespace io {

    namespace {
        const int kqueue_max_event = 256;
    }

    struct kqueue::_descriptor{
        int fd;
        tcode::slist::queue< tcode::operation > op_queue[tcode::io::ev_max];
        std::atomic<int> refcount;

        void complete( kqueue* ep ,  int ev );

        _descriptor( kqueue* ep , int fd  );

        void add_ref( void );
        void release( kqueue* ep );
    };

    kqueue::_descriptor::_descriptor( kqueue* ep , int fd ) {
        refcount.store(1);
        ep->_engine.active().inc();
        this->fd = fd;
    }

    void kqueue::_descriptor::add_ref( void ) {
        refcount.fetch_add(1);
    }

    void kqueue::_descriptor::release( kqueue* ep ){
        if ( refcount.fetch_sub(1) != 1 ) {
            return; 
        }
        ep->_engine.active().dec();
        
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

    void kqueue::_descriptor::complete( kqueue* mux, int events ) {
        static const int kqueueev[] = {
            EVFILT_READ, EVFILT_WRITE  
        };
        for ( int i = 0 ; i < 2 ; ++i ) {
            if ( events == kqueueev[i] ){
                while ( !op_queue[i].empty() ) {
                    io::operation* op = op_queue[i].front<io::operation>();
                    if ( op->post_proc(mux,this) ) {
                        op_queue[i].pop_front();
                        (*op)();
                    } else {
                        break;
                    }
                }
            }
        }
        if ( !op_queue[tcode::io::ev_read].empty())
            mux->_change( fd , EVFILT_READ , EV_ADD | EV_ENABLE | EV_ONESHOT , this );
        if ( !op_queue[tcode::io::ev_write].empty())
            mux->_change( fd , EVFILT_WRITE , EV_ADD | EV_ENABLE | EV_ONESHOT , this );
    }

    kqueue::kqueue( engine& en )
        : _handle( ::kqueue() )
        , _engine( en )
    {
        _change( _wake_up.rd_pipe() , EVFILT_READ , EV_ADD|EV_ENABLE ,nullptr );
    }

    kqueue::~kqueue( void ){
        ::close( _handle );
    }

    int kqueue::run( const tcode::timespan& ts ){
        struct kevent events[kqueue_max_event];
        struct timespec spec;
        spec.tv_sec = ts.total_seconds();
        spec.tv_nsec = ( ts.total_microseconds() % 1000000 ) * 1000;
        std::vector< struct kevent > ch;
        do {
            tcode::threading::spinlock::guard g(_lock);
            ch.swap(_changes);
        }while(0);
        int nofd  = kevent( _handle 
            , &ch[0], ch.size() 
            , events
            , kqueue_max_event
            , &spec );
        if ( nofd <= 0  ) {
            return 0; 
        }
        int run = nofd;
        bool execute_op = false;
        for ( int i = 0 ; i < nofd; ++i ){
            kqueue::descriptor desc =
                static_cast< kqueue::descriptor >( events[i].udata );
            if ( desc ) {
                desc->complete( this , events[i].filter );
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
    
    void kqueue::wake_up( void ){
        char ch=0;
        ::write( _wake_up.wr_pipe() , &ch , 1 );
    }

    bool kqueue::bind( const descriptor& d ) {
        _change( d->fd , EVFILT_READ , EV_ADD | EV_ENABLE | EV_ONESHOT , d );
        _change( d->fd , EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_ONESHOT , d );
        return true;
    }

    void kqueue::unbind( descriptor& d ) {
        do { 
            tcode::threading::spinlock::guard guard(_lock);
            if ( d == nullptr ) 
                return;
            descriptor desc = nullptr;
            std::swap( desc , d );
            op_add( tcode::operation::wrap( 
                    [this,desc] {
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

    void kqueue::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            op_add(op);
        } while(0);
        wake_up();
    }

    void kqueue::connect( 
            kqueue::descriptor& desc 
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
                desc = new kqueue::_descriptor(this , fd );
                desc->op_queue[tcode::io::ev_connect].push_back( op );
                if ( bind( desc )){
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

    void kqueue::write( kqueue::descriptor desc 
            , ip::tcp::operation_write_base* op)
    {
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_write].push_back(op);
                desc->complete( this , EVFILT_WRITE);
                desc->release(this);
            }));
    }

    void kqueue::read( descriptor desc
            , ip::tcp::operation_read_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_read].push_back(op);
                desc->complete( this , EVFILT_READ);
                desc->release(this);
            }));

    }
    
    bool kqueue::listen( descriptor& desc 
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
                desc = new kqueue::_descriptor(this,fd);
                if ( bind( desc ))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void kqueue::accept( descriptor desc
            , int
            , ip::tcp::operation_accept_base* op ){
        desc->add_ref();
        execute( tcode::operation::wrap(
            [this,desc,op]{
                desc->op_queue[tcode::io::ev_accept].push_back(op);
                desc->complete( this , EVFILT_READ);
                desc->release(this);
            }));
    }
    
    bool kqueue::bind( descriptor& desc
            , const ip::address& addr ) 
    {
        int fd = socket( addr.family() , SOCK_DGRAM , IPPROTO_UDP );
        if ( fd != -1 ) {
            tcode::io::ip::option::non_blocking nb;
            nb.set_option(fd);
            tcode::io::ip::option::reuse_address reuse( true );
            reuse.set_option( fd );
            if ( ::bind( fd , addr.sockaddr() , addr.sockaddr_length() ) == 0 ){
                desc = new kqueue::_descriptor( this , fd );
                if ( bind(desc))
                    return true;
                delete desc;
                desc = nullptr;
            }
            ::close(fd);
        }
        return false;
    }

    void kqueue::write( descriptor& desc 
            , ip::udp::operation_write_base* op )
    {   
        if ( desc == nullptr ) {
            int fd = socket( op->address().family() , SOCK_DGRAM , IPPROTO_UDP);
            if ( fd != -1 ){
                desc = new kqueue::_descriptor( this , fd );
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
                desc->complete( this , EVFILT_WRITE);
                desc->release(this);
            }));
    }
    
    void kqueue::read( descriptor desc
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
                desc->complete( this , EVFILT_READ);
                desc->release(this);
            }));
    }


    void kqueue::op_add( tcode::operation* op ){
        _op_queue.push_back( op );
        _engine.active().inc();
    }

    void kqueue::op_run( tcode::operation* op ){
        _engine.active().dec();
        (*op)();
    }

    int kqueue::writev( descriptor desc 
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
                ec = std::error_code();
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int kqueue::readv( descriptor desc 
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
                    ec = std::error_code();
                else
                    ec = tcode::last_error();
            }
        }
        return -1;
    }

    int kqueue::accept( descriptor listen 
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
                accepted = new kqueue::_descriptor( this , fd );
                bind( accepted );
                return 0;
            }
            if ( ( errno == EAGAIN ) || ( errno == EWOULDBLOCK ))
                ec = std::error_code(); 
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int kqueue::read( descriptor desc 
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
                ec = std::error_code(); 
            else
                ec = tcode::last_error();
        }
        return -1;
    }

    int kqueue::write( descriptor desc 
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
                ec = std::error_code(); 
            else
                ec = tcode::last_error();
        }
        return -1;
    }


    void kqueue::_change( int id , int filt , int flag , void* p ) {
        struct kevent e;
        e.ident = id; 
        e.filter = filt;
        e.flags = flag;
        e.fflags = 0;
        e.data = 0;
        e.udata = p;
        do {
            tcode::threading::spinlock::guard guard( _lock );
            _changes.push_back( e );
        }while(0);
    }
}}
