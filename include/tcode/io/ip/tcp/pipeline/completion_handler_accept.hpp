#ifndef __tcode_transport_tcp_comepletion_handler_accept_h__
#define __tcode_transport_tcp_comepletion_handler_accept_h__

#include <io/completion_handler.hpp>
#include <io/ip/address.hpp>

namespace tcode { namespace transport { namespace tcp {

class acceptor;
class completion_handler_accept
	: public tcode::io::completion_handler
{
public:
	completion_handler_accept( acceptor& pacceptor );
	virtual ~completion_handler_accept( void );
	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );

	SOCKET handle( void );

	void handle( SOCKET s );
		
	tcode::io::ip::address* address_ptr( void );
private:
	acceptor& _acceptor;
	SOCKET _handle;
	tcode::io::ip::address _address[2];
};

}}}


#endif