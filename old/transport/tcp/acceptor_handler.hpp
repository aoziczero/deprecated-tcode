#ifndef __tcode_transport_tcp_acceptor_handler_h__
#define __tcode_transport_tcp_acceptor_handler_h__

#include <io/ip/basic_socket.hpp>
#include <transport/event_loop.hpp>
#include <transport/tcp/pipeline_builder.hpp>

namespace tcode { namespace transport { namespace tcp {

class acceptor_handler 
	: public pipeline_builder {
public:
	acceptor_handler( void );
	virtual ~acceptor_handler( void );

	virtual bool condition( tcode::io::ip::socket_type h 
			, const tcode::io::ip::address& addr );
	virtual void acceptor_on_error( const tcode::diagnostics::error_code& ec );
};

typedef tcode::rc_ptr< acceptor_handler > acceptor_handler_ptr;

}}}


#endif