#include "stdafx.h"
#include <tcode/io/epoll.hpp>
#include <tcode/io/option.hpp>
#include <sys/epoll.h>
#include <unistd.h>

namespace tcode { namespace io {

    struct epoll::handle_data {
        int fd;

    };

    epoll::epoll( void )
        : _epoll( epoll_create(256))
    {
    }

    epoll::~epoll( void ){
        ::close( _epoll );
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
