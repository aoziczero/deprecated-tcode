#ifndef __tcode_transport_tcp_channel_acceptor_h__
#define __tcode_transport_tcp_channel_acceptor_h__

#include <transport/event_loop.hpp>
#include <io/ip/basic_socket.hpp>
#include <diagnostics/tcode_error_code.hpp>

#include <transport/tcp/pipeline_builder.hpp>
#if defined( TCODE_TARGET_LINUX )
#include <io/epoll.hpp>
#endif

namespace tcode { namespace transport { namespace tcp {

class completion_handler_accept;
class acceptor 
	: public tcode::io::ip::accept_base
#if defined( TCODE_TARGET_LINUX )
	, public tcode::io::epoll::handler
#endif
{
public:
	acceptor(tcode::transport::event_loop& l);
	virtual ~acceptor(void);		

	bool listen( const tcode::io::ip::address& bind_addr
		, pipeline_builder_ptr builder );

	void close( void );

	tcode::transport::event_loop& loop( void );
	
	virtual bool condition( tcode::io::ip::socket_type h 
			, const tcode::io::ip::address& addr );
	virtual void on_error( const tcode::diagnostics::error_code& ec );
	
#if defined (TCODE_TARGET_WINDOWS )
	void do_accept(completion_handler_accept* h);
	void handle_accept( const tcode::diagnostics::error_code& ec 
				, completion_handler_accept* handler );
#elif defined( TCODE_TARGET_LINUX )
	
	virtual void operator()( const int events);
	void handle_accept( void );
#endif
private:
	tcode::transport::event_loop& _loop;
	pipeline_builder_ptr _builder;
	int _address_family;
	
};

}}}

#endif
