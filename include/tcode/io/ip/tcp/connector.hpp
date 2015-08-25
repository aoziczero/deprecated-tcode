#ifndef __tcode_io_ip_tcp_connector_h__
#define __tcode_io_ip_tcp_connector_h__

#include <tcode/io/ip/ip.hpp>

namespace tcode { namespace io { 

    class engine;

namespace ip { namespace tcp {

    class socket;
    class connector {
    public:
        connector( socket& s );
        ~connector( void );
        /*
        ip::socket_type connect( const ip::address& addr 
                , const tcode::timespan& ts ); 
                */
    private:
        socket& _socket;
    };

}}}}

#endif
