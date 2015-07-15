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
	class error_handler : public tcode::ref_counted< error_handler >{
	public:
		virtual ~error_handler(){}
		virtual void operator()( const tcode::diagnostics::error_code& ec ){}
	};

	virtual ~connector(void);		

	bool connect_timeout( const tcode::io::ip::address& conn_addr
		, pipeline_builder_ptr builder 
		, const tcode::time_span& ts );

	bool connect( const tcode::io::ip::address& addr
		, pipeline_builder_ptr builder
		, const tcode::time_span& ts );

	tcode::transport::event_loop& loop( void );
	
	bool do_connect(void);
	void handle_connect( const tcode::diagnostics::error_code& ec  );

#if defined( TCODE_TARGET_WINDOWS )	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );
#elif defined( TCODE_TARGET_LINUX )
	virtual void operator()( const int events );
#endif
private:
	tcode::transport::event_loop& _loop;
	pipeline_builder_ptr _builder;
	tcode::io::ip::address _address;
	event_timer::pointer_type _timer;
	bool _timeout;
	tcode::rc_ptr< error_handler > _err_handler;
private:
	connector(tcode::transport::event_loop& l , const tcode::rc_ptr< error_handler >& h );
public:
	static tcode::rc_ptr<connector> create(
		tcode::transport::event_loop& l 
		, const tcode::rc_ptr< error_handler >& errh
	);
};

}}}

#endif
