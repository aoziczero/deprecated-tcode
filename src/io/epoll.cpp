#include "stdafx.h"
#include <tcode/io/epoll.hpp>
#include <tcode/io/option.hpp>
#include <sys/epoll.h>
#include <unistd.h>
#include <tcode/io/reactor_completion_handler.hpp>

namespace tcode { namespace io {
namespace {
    const int epoll_max_event = 256;
}

    struct epoll::descriptor{
        int fd;
        tcode::slist::queue< reactor_completion_handler > handler_queue[2];


        void complete( int ev );
    };

    void epoll::descriptor::complete( int ev ) {
        int epollev[] = { EPOLLIN , EPOLLOUT };
        for ( int i = 0 ; i < 2 ; ++i ) {
            if ( !epollev[i] & ev ) 
                continue;
            if ( handler_queue[i].empty() )
                continue;
            while ( !handler_queue[i].empty() ) {
                reactor_completion_handler* rch = handler_queue[i].front();
                if ( !(*rch)()) {
                    break;
                }
                handler_queue[i].pop_front();
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
        for ( int i = 0 ; i < nofd; ++i ){
            epoll::descriptor_type desc =
                static_cast< epoll::descriptor_type >( events[i].data.ptr );
            if ( desc ) {
                desc->complete( events[i].events );
            }
        }
        return nofd;
    }
    
    void epoll::wake_up( void ){
        struct epoll_event e;
        e.events = EPOLLOUT | EPOLLONESHOT;
        e.data.ptr = nullptr;
        epoll_ctl( _handle , EPOLL_CTL_MOD , _wake_up.wr_pipe() , &e );
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
