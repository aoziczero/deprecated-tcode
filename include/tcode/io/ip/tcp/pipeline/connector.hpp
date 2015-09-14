#ifndef __tcode_io_ip_tcp_pipeline_connector_h__
#define __tcode_io_ip_tcp_pipeline_connector_h__

#include <tcode/time/timespan.hpp>
#include <tcode/io/ip/tcp/pipeline/connector_handler.hpp>
#include <vector>
#include <memory>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class connector {
    public:	
        connector(void);
        ~connector(void);		

        void connect( const tcode::io::ip::address& addr
                , const tcode::timespan& wait_time 
                , const std::shared_ptr< connector_handler >& handler );

        void connect_sequence( const std::vector< tcode::io::ip::address >& addr 
                , const tcode::timespan& per_wait_time 
                , const std::shared_ptr< connector_handler >& handler );
    private:
        std::shared_ptr< connector_handler > _handler;
    };

}}}}

#endif
