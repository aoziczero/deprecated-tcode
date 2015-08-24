#ifndef __tcode_io_ip_h__
#define __tcode_io_ip_h__

#include <tcode/tcode.hpp>
#include <tcode/time/timestamp.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/io/ip/address.hpp>

#if defined( TCODE_WIN32 )

#else
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

namespace tcode { namespace io { namespace ip {

#if defined( TCODE_WIN32 ) 
    typedef SOCKET socket_type;	
#else
    typedef int socket_type;
#endif

#if defined( TCODE_WIN32 ) 
    extern socket_type invalid_socket;
    extern socket_type socket_error;
#else
    extern socket_type invalid_socket;
    extern socket_type socket_error;
#endif

    socket_type socket( int family , int type , int proto );
    void        close( socket_type fd ); 

}}}

#endif
