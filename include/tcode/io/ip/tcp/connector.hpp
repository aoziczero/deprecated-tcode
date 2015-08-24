#ifndef __tcode_io_ip_tcp_connector_h__
#define __tcode_io_ip_tcp_connector_h__

#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { 

    class engine;

namespace ip { namespace tcp {

    class connector {
    public:
        int connnect( const address& addr , const tcode::timespan& ts ); 
    private:

    };

}}}}

#endif
