#include "stdafx.h"
#include <tcode/io/option.hpp>

#if defined( TCODE_WIN32 )

#else
#include <unistd.h>
#include <fcntl.h>
#endif
namespace tcode { namespace io { namespace option {

#if defined( TCODE_WIN32 )
    bool block::set_option( SOCKET fd ) {
        unsigned long opt = 0;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
    }

    bool nonblock::set_option( SOCKET fd ) {
        unsigned long opt = 1;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
    }

#else
    bool block::set_option( int fd ) {
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt & ~O_NONBLOCK ) != -1; 
    }
    
    bool nonblock::set_option( int fd ){
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt | O_NONBLOCK ) != -1; 
    }
#endif
    
}}}
