#include "stdafx.h"
#include <tcode/io/epoll.hpp>
#include <tcode/io/option.hpp>
#include <sys/epoll.h>
#include <unistd.h>
#include <tcode/io/reactor_completion_handler.hpp>
#include <tcode/io/ip/tcp/reactor_completion_handler_connect.hpp>

namespace tcode { namespace io {
namespace {
    const int epoll_max_event = 256;
}

    struct epoll::_descriptor{
        int fd;
        tcode::slist::queue< reactor_completion_handler > handler_queue[2];


        void complete( int ev );
    };

    void epoll::_descriptor::complete( int events ) {
        int epollev[] = { EPOLLIN , EPOLLOUT };
        for ( int i = 0 ; i < 2 ; ++i ) {
            if ( events & epollev[i] ){
                while ( !handler_queue[i].empty() ) {
                    reactor_completion_handler* rch = 
                       handler_queue[i].front();
                    if ( rch->do_reactor_io(this) ) {
                        handler_queue[i].pop_front();
                        rch->complete(this);
                    } else {
                        break;
                    }
                }
            }
        }
    }

    epoll::epoll( void )
        : _handle( epoll_create(epoll_max_event))
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
        bool execute_op = false;
        for ( int i = 0 ; i < nofd; ++i ){
            epoll::descriptor desc =
                static_cast< epoll::descriptor >( events[i].data.ptr );
            if ( desc ) {
                desc->complete( events[i].events );
            } else {
                execute_op = true;
            }
        }
        if ( execute_op ){
            tcode::slist::queue< tcode::operation > ops;
            do {
                tcode::threading::spinlock::guard guard(_lock);
                if ( _op_queue.empty() )
                    return 0;
                ops.splice( _op_queue );
            }while(0);
            while( !ops.empty() ){
                tcode::operation* op = ops.front();
                ops.pop_front();
                (*op)();
            }
        }
        return 0;
    }
    
    void epoll::wake_up( void ){
        struct epoll_event e;
        e.events = EPOLLOUT | EPOLLONESHOT;
        e.data.ptr = nullptr;
        epoll_ctl( _handle , EPOLL_CTL_MOD , _wake_up.wr_pipe() , &e );
    }

    bool epoll::bind( int fd , descriptor& d ) {
        d = new _descriptor();
        d->fd = fd;
        struct epoll_event e;
        e.events = EPOLLIN | EPOLLOUT | EPOLLET;
        e.data.ptr = d;
        return epoll_ctl( _handle , EPOLL_CTL_ADD , fd , &e ) == 0;
    }

    void epoll::unbind( descriptor& d ) {
        if ( d == nullptr )
            return;

        epoll_ctl( _handle , EPOLL_CTL_DEL , d->fd , nullptr );
        ::close( d->fd );
        
        tcode::slist::queue< reactor_completion_handler > handlers;
        handlers.splice( d->handler_queue[0] );
        handlers.splice( d->handler_queue[1] );

        delete d;
        d = nullptr;
        if ( handlers.empty() ) {
            return; 
        }
        execute( tcode::operation::wrap(
                    [ handlers ]{
                        tcode::slist::queue< reactor_completion_handler > ops(
                               handlers );
                        while ( !ops.empty()){
                            reactor_completion_handler* ch = ops.front();
                            ops.pop_front();
                            ch->complete( nullptr );
                        }
                    }));
    }

    void epoll::execute( tcode::operation* op ) {
        tcode::threading::spinlock::guard guard(_lock);
        _op_queue.push_back(op);
        wake_up();
    }

    void epoll::connect( 
            epoll::descriptor& desc 
            , ip::tcp::reactor_completion_handler_connect_base* op )
    {

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
