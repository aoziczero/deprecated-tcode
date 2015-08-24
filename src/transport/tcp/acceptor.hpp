#ifndef __tcode_transport_tcp_channel_acceptor_h__
#define __tcode_transport_tcp_channel_acceptor_h__

#include <transport/tcp/acceptor_handler.hpp>
#include <diagnostics/tcode_error_code.hpp>
#include <diagnostics/log/log.hpp>

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
		, const acceptor_handler_ptr& handler );

	void close( void );

	tcode::transport::event_loop& loop( void );
	
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
	acceptor_handler_ptr _handler;
	int _address_family;	
public:
	static diagnostics::log::logger& logger();
};

}}}

#endif
