#ifndef __tcode_io_ip_tcp_acceptor_handler_h__
#define __tcode_io_ip_tcp_acceptor_handler_h__

#include <tcode/io/ip/tcp/pipeline/pipeline_builder.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

class acceptor_handler 
	: public pipeline_builder {
public:
	acceptor_handler( void );
	virtual ~acceptor_handler( void );

	virtual bool condition( const tcode::io::ip::address& addr );
	virtual void acceptor_on_error( const tcode::diagnostics::error_code& ec );
};

}}}}

#endif
