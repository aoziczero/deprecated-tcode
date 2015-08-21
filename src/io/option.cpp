#include "stdafx.h"
#include <tcode/io/option.hpp>

#if defined( TCODE_WIN32 )

#else
#include <unistd.h>
#include <fcntl.h>
#endif
namespace tcode { namespace io { namespace option {

    bool block::set_option( native_handle fd )
    {
#if defined( TCODE_WIN32 )
        unsigned long opt = 0;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
#else
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt & ~O_NONBLOCK ) != -1; 
#endif
    }
 
    bool nonblock::set_option( native_handle fd )
    {
#if defined( TCODE_WIN32 )
        unsigned long opt = 1;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
#else
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt | O_NONBLOCK ) != -1; 
#endif
    }

    linger_remove_time_wait::linger_remove_time_wait( void )
    {
        option().l_onoff = 1;
        option().l_linger = 0;
    }

}}}
