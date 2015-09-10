#ifndef __tcode_io_ip_tcp_pipeline_channel_h__
#define __tcode_io_ip_tcp_pipeline_channel_h__

#include <tcode/io/ip/tcp/socket.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp { namespace pipeline {

    class channel {
    public:
       channel( socket&& s );
       ~channel( void );
    private:
        tcode::io::ip::tcp::socket _socket;
    };

}}}}}

#endif
