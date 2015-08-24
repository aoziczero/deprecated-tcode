#include "stdafx.h"
#include <tcode/io/ip/simple_select.hpp>
#if defined( TCODE_WIN32 )

#else
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

namespace tcode { namespace io { namespace ip {

    bool selector::wait_for_read( socket_type fd 
            , const tcode::timespan& wait  ){
        if ( fd  == invalid_socket ) {
            return false;
        }
        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET( fd , &fdset);

        timeval tval;
        tval.tv_sec = static_cast<int>(wait.total_seconds());
        tval.tv_usec = wait.total_microseconds() % 1000 * 1000;
#if defined( TCODE_WIN32 )
        int result = ::select( 0 , &fdset , NULL , NULL , &tval );
        if( result <= 0 ) { // -1 error 0 timeout
            return false;
        }	
#else
        while( true ) {
            int ret = select( fd  + 1 ,&fdset , NULL ,  NULL , &tval );
            if ( ret > 0 )  break;
            if ( ret == -1 && errno == EINTR ) continue;
            if( ret <= 0 ) {  // -1 error 0 timeout
                return false;
            }
        }
#endif
        return FD_ISSET( fd , &fdset ) != 0;
    }

    bool selector::wait_for_write( socket_type fd 
            , const tcode::timespan& wait  ){
        if ( fd == invalid_socket ) {
            return false;
        }

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET( fd , &fdset);

        timeval tval;
        tval.tv_sec = static_cast<int>(wait.total_seconds());
        tval.tv_usec = wait.total_microseconds() % 1000 * 1000;
#if defined( TCODE_WIN32 )
        int result = ::select( 0 , NULL , &fdset , NULL , &tval );
        if( result <= 0 ) {  // -1 error 0 timeout
            return false;
        }
#else
        while( true ) {
            int ret = select(fd + 1 , NULL , &fdset ,  NULL , &tval );
            if ( ret > 0 )  break;
            if ( ret == -1 && errno == EINTR ) continue;
            if( ret <= 0 ) {  // -1 error 0 timeout
                return false;
            }
        }    
#endif
        return FD_ISSET( fd  , &fdset ) != 0;
    }

}}}

