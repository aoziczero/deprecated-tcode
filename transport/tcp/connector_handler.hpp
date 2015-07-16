#ifndef __tcode_transport_tcp_connector_handler_h__
#define __tcode_transport_tcp_connector_handler_h__

#include <io/ip/basic_socket.hpp>
#include <transport/event_loop.hpp>
#include <transport/tcp/pipeline_builder.hpp>

namespace tcode { namespace transport { namespace tcp {

#if defined( TCODE_TARGET_WINDOWS )
class completion_handler_connect;
#endif
class connector_handler 
	: public pipeline_builder
	, public tcode::io::ip::connect_base
#if defined( TCODE_TARGET_LINUX )
	, public tcode::io::epoll::handler
#endif
 {
public:
	connector_handler( void );
	virtual ~connector_handler( void );

	virtual void connector_on_error( const tcode::diagnostics::error_code& ec );
	virtual bool connector_do_continue( void ) = 0;
	
	tcode::time_span timeout( void );
	void timeout( const tcode::time_span& timeout );

	bool do_connect(const std::vector< tcode::io::ip::address >& addrs );
	void do_connect();
	void handle_connect( const tcode::diagnostics::error_code& ec  );

	void on_timer( const tcode::diagnostics::error_code& ec );
#if defined( TCODE_TARGET_LINUX )
	virtual void operator()( const int events );
#endif
private:
	std::vector< tcode::io::ip::address > _address;
	tcode::transport::event_loop* _channel_loop;
	int _current_address;
	event_timer_ptr _timer;
	tcode::time_span _timeout;
	tcode::time_stamp _connect_time;
#if defined( TCODE_TARGET_WINDOWS )
	completion_handler_connect* _completion_handler;
#endif
};

typedef tcode::rc_ptr< connector_handler > connector_handler_ptr;

}}}


#endif