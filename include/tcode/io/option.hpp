#ifndef __tcode_io_option_h__
#define __tcode_io_option_h__

namespace tcode { namespace io { namespace option {

    class block{
    public:
        bool set_option( 
#if defined( TCODE_WIN32 )
                SOCKET fd
#else
                int fd
#endif
                 );
    };

    class nonblock{
    public:
        bool set_option(
#if defined( TCODE_WIN32 )
                SOCKET fd
#else
                int fd
#endif
                );      
    };
}
}}

#endif
