#ifndef __tcode_transport_tcp_channel_connector_h__
#define __tcode_transport_tcp_channel_connector_h__

#include <diagnostics/tcode_error_code.hpp>
#include <transport/tcp/connector_handler.hpp>
#include <transport/event_timer.hpp>

namespace tcode { namespace transport { namespace tcp {

class connector {
public:	
	connector(void);
	~connector(void);		

	void connect( const tcode::io::ip::address& addr
		, const tcode::time_span& wait_time 
		, const connector_handler_ptr handler );

	void connect_sequence( const std::vector< tcode::io::ip::address >& addr 
		, const tcode::time_span& per_wait_time 
		, const connector_handler_ptr handler );
};

}}}

#endif
