#ifndef __tcode_transport_tcp_channel_acceptor_h__
#define __tcode_transport_tcp_channel_acceptor_h__

#include <transport/event_loop.hpp>
#include <io/ip/basic_socket.hpp>
#include <diagnostics/tcode_error_code.hpp>

namespace tcode { namespace transport { namespace tcp {

class accept_completion_handler;
class pipeline_builder;
class acceptor 
	: public tcode::io::ip::accept_base
{
public:
	acceptor(tcode::transport::event_loop& l);
	virtual ~acceptor(void);		

	bool listen( const tcode::io::ip::address& bind_addr
		, pipeline_builder* builder );

	tcode::transport::event_loop& loop( void );
	
	virtual bool condition( tcode::io::ip::socket_type h 
			, const tcode::io::ip::address& addr );
	virtual void on_error( const tcode::diagnostics::error_code& ec );
	
	void do_accept(accept_completion_handler* h);
	void handle_accept( const tcode::diagnostics::error_code& ec 
				, accept_completion_handler* handler );
private:
	tcode::transport::event_loop& _loop;
	pipeline_builder* _builder;
	int _address_family;
	
};

}}}

#endif
