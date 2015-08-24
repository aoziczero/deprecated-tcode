#include "stdafx.h"
#include <tcode/io/ip/ip.hpp>

namespace tcode { namespace io { namespace ip {
#if defined( TCODE_WIN32 ) 
    socket_type invalid_socket = INVALID_SOCKET;
    socket_type socket_error = SOCKET_ERROR;
#else
    socket_type invalid_socket =-1;
    socket_type socket_error = invalid_socket;
#endif

    socket_type socket( int family , int type , int proto ){
#if defined( TCODE_WIN32 )
        return WSASocket( family , type , proto , nullptr , 0 , WSA_FLAG_OVERLAPPED );
#else
        return ::socket( family , type , proto );
#endif

    }

    void        close( socket_type fd ){
        if ( fd != invalid_socket ) {
#if defined( TCODE_WIN32 )
            closesocket( fd );
#else
            ::close( fd );   
#endif
        }
    }


}}}
