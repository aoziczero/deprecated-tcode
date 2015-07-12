#ifndef __tcode_transport_tcp_channel_connector_h__
#define __tcode_transport_tcp_channel_connector_h__

#include <io/ip/basic_socket.hpp>
#include <diagnostics/tcode_error_code.hpp>
#include <transport/event_loop.hpp>
#include <transport/tcp/pipeline_builder.hpp>
#include <transport/event_timer.hpp>

namespace tcode { namespace transport { namespace tcp {

class connector 
	: public tcode::io::ip::connect_base
#if defined( TCODE_TARGET_WINDOWS )	
	, public tcode::io::completion_handler
#elif defined( TCODE_TARGET_LINUX )
	, public tcode::io::epoll::handler
#endif
	, public tcode::ref_counted< connector >
{
public:
	connector(tcode::transport::event_loop& l);
	virtual ~connector(void);		

	bool connect_timeout( const tcode::io::ip::address& conn_addr
		, pipeline_builder_ptr builder 
		, const tcode::time_span& ts );

	bool connect( const tcode::io::ip::address& addr
		, pipeline_builder_ptr builder
		, const tcode::time_span& ts );

	tcode::transport::event_loop& loop( void );
	
	virtual void on_error( const std::error_code& ec );
	
	bool do_connect(void);
	void handle_connect( const tcode::diagnostics::error_code& ec  );
private:
	tcode::transport::event_loop& _loop;
	pipeline_builder_ptr _builder;
	tcode::io::ip::address _address;
	event_timer::pointer_type _timer;
	bool _timeout;
private:
#if defined( TCODE_TARGET_WINDOWS )	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );
#elif defined( TCODE_TARGET_LINUX )
	virtual void operator()( const int events );
#endif
};


/*
class completion_handler_connect;
typedef tcode::rc_ptr< completion_handler_connect > connect_id;

class connector {
public:
	class event_handler 
		: public tcode::ref_counted< event_handler > {
	public:
		virtual ~event_handler( void ){}
		virtual void on_connect( const tcode::io::ip::address& addr ){
		}
		virtual void on_error( const tcode::diagnostics::error_code& ec ){
		}
	};	
	typedef tcode::rc_ptr< event_handler > event_handler_ptr;

	connector(tcode::transport::event_loop& l);
	~connector(void);		
	
	// blocking call
	bool connect_timeout( const tcode::io::ip::address& conn_addr
		, pipeline_builder_ptr builder 
		, const tcode::time_span& ts );

	connect_id connect(const tcode::io::ip::address& conn_addr
		, pipeline_builder_ptr builder 
		, event_handler_ptr errhandler
		, const tcode::time_span& ts );
private:
	tcode::transport::event_loop& _loop;	
};
*/

}}}

#endif
