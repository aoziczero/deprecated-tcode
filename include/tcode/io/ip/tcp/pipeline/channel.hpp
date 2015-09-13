#ifndef __tcode_io_ip_tcp_pipeline_channel_h__
#define __tcode_io_ip_tcp_pipeline_channel_h__

#include <tcode/io/ip/tcp/socket.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp { namespace pipeline {

    class channel {
    public:
       channel( socket&& s );
       ~channel( void );

       void add_ref( void );
       void release( void );
    private:
       std::atomic<int> _refcount;
       tcode::io::ip::tcp::socket _socket;
    };

}}}}}

#endif
