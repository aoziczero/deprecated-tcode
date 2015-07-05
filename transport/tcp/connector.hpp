#ifndef __tcode_transport_tcp_channel_connector_h__
#define __tcode_transport_tcp_channel_connector_h__

#include <transport/event_loop.hpp>
#include <io/ip/basic_socket.hpp>
#include <diagnostics/tcode_error_code.hpp>


namespace tcode { namespace transport { namespace tcp {

class pipeline_builder;
class completion_handler_connect;
typedef tcode::rc_ptr< completion_handler_connect > connect_id;

class connector 
{
public:
	connector(tcode::transport::event_loop& l);
	~connector(void);		
	
	// blocking call
	bool connect_timeout( const tcode::io::ip::address& conn_addr
		, pipeline_builder* builder 
		, const tcode::time_span& ts );

	connect_id connect(const tcode::io::ip::address& conn_addr
		, pipeline_builder* builder 
		, const tcode::time_span& ts );
private:
	tcode::transport::event_loop& _loop;	
};

}}}

#endif
