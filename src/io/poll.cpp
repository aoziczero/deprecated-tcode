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

}}
