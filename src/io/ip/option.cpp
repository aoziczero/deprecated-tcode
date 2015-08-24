#include "stdafx.h"
#include <tcode/io/ip/option.hpp>

namespace tcode { namespace io { namespace ip { namespace option {
    
   bool blocking::set_option( socket_type fd ){
#if defined( TCODE_WIN32)
        unsigned long opt = 0;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
#else
        int x = fcntl( fd , F_GETFL , 0);
        return fcntl(fd , F_SETFL , x & ~O_NONBLOCK ) != -1;
#endif
    }
   
    bool non_blocking::set_option( socket_type fd ){
#if defined( TCODE_WIN32)
        unsigned long opt = 1;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
#else
        int x = fcntl( fd , F_GETFL , 0);
        return fcntl(fd , F_SETFL , x | O_NONBLOCK )!= -1;
#endif
    }

    linger_remove_time_wait::linger_remove_time_wait( void ){
        linger::value().l_onoff = 1;
        linger::value().l_linger = 0;
    }


}}}}
