#ifndef __tcode_io_ip_tcp_pipeline_connector_h__
#define __tcode_io_ip_tcp_pipeline_connector_h__

#include <tcode/io/ip/tcp/socket.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp { namespace pipeline {

    class connector {
    public:
       connector( socket&& s );
       ~connector( void );
    private:
        tcode::io::ip::tcp::socket _socket;
    };

}}}}}

#endif
