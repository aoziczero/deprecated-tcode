#include "stdafx.h"
#include <tcode/io/epoll.hpp>
#include <tcode/io/ip/option.hpp>
#include <sys/epoll.h>
#include <unistd.h>
#include <tcode/io/operation.hpp>
#include <tcode/io/ip/tcp/operation_connect.hpp>

namespace tcode { namespace io {
namespace {
    const int epoll_max_event = 256;
}

    struct epoll::_descriptor{
        int fd;
        tcode::slist::queue< tcode::operation > op_queue[2];
        void complete( int ev );
    };

    void epoll::_descriptor::complete( int events ) {
        int epollev[] = { EPOLLIN , EPOLLOUT };
        for ( int i = 0 ; i < 2 ; ++i ) {
            if ( events & epollev[i] ){
                while ( !op_queue[i].empty() ) {
                    io::operation* op = static_cast< io::operation* >( 
                       op_queue[i].front());
                    if ( op->post_proc(this) ) {
                        op_queue[i].pop_front();
                        (*op)();
                    } else {
                        break;
                    }
                }
            }
        }
    }

    epoll::epoll( active_ref& ac )
        : _handle( epoll_create(epoll_max_event))
        , _active( ac )
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
                desc->complete( events[i].events );
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
                ops.splice( _op_queue );
            }while(0);
            while( !ops.empty() ){
                tcode::operation* op = ops.front();
                ops.pop_front();
                (*op)();
                _active.dec();
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

    bool epoll::bind( descriptor d ) {
        _active.inc();
        struct epoll_event e;
        e.events = EPOLLIN | EPOLLOUT | EPOLLET;
        e.data.ptr = d;
        return epoll_ctl( _handle , EPOLL_CTL_ADD , d->fd , &e ) == 0;
    }

    void epoll::unbind( descriptor& d ) {
        descriptor desc;
        do { 
            tcode::threading::spinlock::guard guard(_lock);
            if ( d == nullptr ) 
                return;
            desc = d;
            d = nullptr;
        }while(0);

        if ( desc->fd != -1 ) {
            epoll_ctl( _handle , EPOLL_CTL_DEL , desc->fd , nullptr );
            ::close( desc->fd );
            desc->fd = -1;
        }
        
        tcode::slist::queue< tcode::operation > ops;
        for ( int i = 0 ;i < 2 ; ++i ) {
            while ( !desc->op_queue[i].empty()){
                tcode::io::operation* op = static_cast< io::operation* >(
                        desc->op_queue[i].front());
                desc->op_queue[i].pop_front();
                op->error() = tcode::error_aborted;
                ops.push_back(op);
            }
        }

        delete desc;
        _active.dec();

        if ( ops.empty() ) return;
       
        do {
            tcode::threading::spinlock::guard guard(_lock);
            while( !ops.empty()){
                tcode::operation* op = ops.front();
                ops.pop_front();
                _op_queue.push_back( op );
                _active.inc();
            }
        }while(0);
        wake_up();
    }

    void epoll::execute( tcode::operation* op ) {
        do {
            tcode::threading::spinlock::guard guard(_lock);
            _op_queue.push_back(op);
        } while(0);
        _active.inc();
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
            epoll::_descriptor* d = new epoll::_descriptor();
            d->fd = fd;
            d->op_queue[1].push_back( op );
            desc = d;
            bind( d );
        } else {
            ::close(fd);
            op->error() = tcode::last_error();
            execute(op);
        }
    }

/*
    epoll::epoll( void )
        : _pipe_handler( [this] ( int ev ) {
                wake_up_handler( ev );
            }) 
    {
        _handle = epoll_create( 256 );
        tcode::io::option::nonblock nb;
        nb.set_option( _pipe.rd_pipe());
        nb.set_option( _pipe.wr_pipe());
        bind( _pipe.rd_pipe() , EV_READ , &_pipe_handler );
    }


    epoll::~epoll( void ) {
        close( _handle );
    }
        
    bool epoll::bind( int fd , int ev , tcode::io::event_handler* handler ){
        int epollev = 0;
        if ( ev & EV_READ ) epollev = EPOLLIN;
        if ( ev & EV_WRITE ) epollev |= EPOLLOUT;
        
        epoll_event evt;
        evt.events = epollev;
        evt.data.ptr = handler;
        
        if ( epoll_ctl( _handle 
                    , EPOLL_CTL_MOD
                    , fd
                    , &evt) == 0 )
            return true;
        if ( errno == ENOENT )
            return epoll_ctl( _handle 
                    , EPOLL_CTL_ADD
                    , fd
                    , &evt) == 0;
        return false;
    }

    void epoll::unbind( int fd ){
	    epoll_ctl( _handle , EPOLL_CTL_DEL , fd , nullptr );
    }

    int epoll::run( const tcode::timespan& ts ) {
        struct epoll_event events[256];
        int nofd  = epoll_wait( _handle 
            , events
            , 256 
            , static_cast<int>(ts.total_milliseconds()));
        if ( nofd == -1 ) {
            return errno == EINTR ? 0 : -1;
        } else {
            for ( int i = 0 ; i < nofd; ++i ){
                tcode::io::event_handler* handler =
                        static_cast< tcode::io::event_handler* >( events[i].data.ptr );
                if ( handler ) {
                    int ev = 0;
                    if ( events[i].events & EPOLLIN )
                        ev = EV_READ;
                    if ( events[i].events & EPOLLOUT )
                        ev |= EV_WRITE;

                    (*handler)( ev );
                }
            }
        }
        return 0;
    }

    void epoll::wake_up( void ){
        char ch=0;
        write( _pipe.wr_pipe() , &ch , 1 );
    }

    void epoll::wake_up_handler( int ev ){
        char ch;
        while ( read( _pipe.rd_pipe() , &ch , 1 ) == 1 )
            ;
    }
    */

    
}}
