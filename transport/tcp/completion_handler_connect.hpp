#ifndef __tcode_transport_tcp_comepletion_handler_connect_h__
#define __tcode_transport_tcp_comepletion_handler_connect_h__

#include <common/rc_ptr.hpp>

#include <buffer/byte_buffer.hpp>

#include <io/io.hpp>
#include <io/completion_handler.hpp>
#include <io/ip/basic_socket.hpp>

#include <transport/event_timer.hpp>
#include <transport/tcp/pipeline_builder.hpp>

#include <transport/tcp/connector.hpp>


#if defined( TCODE_TARGET_LINUX )
#include <io/epoll.hpp>
#endif

namespace tcode { namespace transport { namespace tcp {
/*
class completion_handler_connect
	: public tcode::io::completion_handler
#if defined( TCODE_TARGET_LINUX )
	, public tcode::io::epoll::handler
#endif
	, public tcode::ref_counted< completion_handler_connect >
{
public:
	completion_handler_connect( tcode::transport::event_loop& l );
	virtual ~completion_handler_connect( void );	

	void connect( const tcode::io::ip::address& conn_addr
			, pipeline_builder_ptr builder 
			, connector::event_handler_ptr ptr
			, const tcode::time_span& ts);
	void on_timer( void );
	void cancel( void );
	void handle_connect( const tcode::diagnostics::error_code& ec );
#if defined( TCODE_TARGET_WINDOWS)
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );
#elif defined( TCODE_TARGET_LINUX )
	bool do_connect( void );
	virtual void operator()( const int events );
#endif
private:
	tcode::transport::event_loop& _loop;
	pipeline_builder_ptr _builder;
	tcode::io::ip::connect_base _handle;
	std::atomic< int > _flag;	
	tcode::transport::event_timer::pointer_type _timer;
	tcode::io::ip::address _conn_addr;
	connector::event_handler_ptr _event_handler;
};
*/
}}}


#endif