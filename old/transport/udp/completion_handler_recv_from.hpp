#ifndef __tcode_transport_udp_comepletion_handler_read_h__
#define __tcode_transport_udp_comepletion_handler_read_h__

#include <buffer/byte_buffer.hpp>
#include <io/completion_handler.hpp>
#include <io/io.hpp>
#include <io/ip/address.hpp>

namespace tcode { namespace transport { namespace udp {

class channel;
class completion_handler_recv_from
	: public tcode::io::completion_handler
{
public:
	completion_handler_recv_from( channel& channel );
	virtual ~completion_handler_recv_from( void );	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes );
	
	tcode::iovec read_buffer( const int sz );
	tcode::io::ip::address& address( void );
private:
	channel& _channel;
	tcode::io::ip::address _address;
	tcode::buffer::byte_buffer _read_buffer;
};

}}}


#endif