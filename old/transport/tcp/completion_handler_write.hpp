#ifndef __tcode_transport_tcp_comepletion_handler_write_h__
#define __tcode_transport_tcp_comepletion_handler_write_h__

#include <buffer/byte_buffer.hpp>
#include <io/completion_handler.hpp>
#include <io/io.hpp>

namespace tcode { namespace transport { namespace tcp {

class channel;
class completion_handler_write
	: public tcode::io::completion_handler
{
public:
	completion_handler_write( channel& channel );
	virtual ~completion_handler_write( void );	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );
private:
	channel& _channel;
};

}}}


#endif