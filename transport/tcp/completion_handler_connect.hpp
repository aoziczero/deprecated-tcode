#ifndef __tcode_transport_tcp_comepletion_handler_connect_h__
#define __tcode_transport_tcp_comepletion_handler_connect_h__

#include <io/completion_handler.hpp>
#include <transport/tcp/connector_handler.hpp>

namespace tcode { namespace transport { namespace tcp {

class completion_handler_connect
	: public tcode::io::completion_handler
{
public:
	completion_handler_connect( const connector_handler_ptr& ptr );
	virtual ~completion_handler_connect( void );	

	void cancel( void );

	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );
private:
	std::atomic<int> _cancel;
	connector_handler_ptr _handler;	
};

}}}


#endif