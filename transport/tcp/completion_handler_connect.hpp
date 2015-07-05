#ifndef __tcode_transport_tcp_comepletion_handler_connect_h__
#define __tcode_transport_tcp_comepletion_handler_connect_h__

#include <common/rc_ptr.hpp>

#include <buffer/byte_buffer.hpp>

#include <io/io.hpp>
#include <io/completion_handler.hpp>
#include <io/ip/basic_socket.hpp>

#include <transport/event_timer.hpp>
#include <transport/tcp/pipeline_builder.hpp>


#if defined( TCODE_TARGET_LINUX )
#include <io/epoll.hpp>
#endif

namespace tcode { namespace transport { namespace tcp {

class completion_handler_connect
#if defined( TCODE_TARGET_WINDOWS)
	: public tcode::io::completion_handler
#elif defined( TCODE_TARGET_LINUX )
	: public tcode::io::epoll::handler
#endif
	, public tcode::ref_counted< completion_handler_connect >
{
public:
	completion_handler_connect( tcode::transport::event_loop& l 
		, pipeline_builder* builder 
		, const tcode::time_span& ts );
	virtual ~completion_handler_connect( void );	

	void on_timer( void );
#if defined( TCODE_TARGET_WINDOWS)
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );
#elif defined( TCODE_TARGET_LINUX )
	virtual void operator()( const int events );
#endif
private:
	tcode::transport::event_loop& _loop;
	pipeline_builder* _builder;
	tcode::io::ip::tcp_holder _handle;
	std::atomic< int > _flag;	
	tcode::transport::event_timer::pointer_type _timer;
};

}}}


#endif