#ifndef __tcode_diagnostics_log_udp_writer_h__
#define __tcode_diagnostics_log_udp_writer_h__

#include <common/rc_ptr.hpp>

#include <diagnostics/log/writer.hpp>

#include <io/ip/basic_socket.hpp>


namespace tcode { namespace diagnostics { namespace log {

class udp_writer : public writer
{
public:
	udp_writer( const tcode::io::ip::address& endpoint );
	virtual ~udp_writer( void );
	virtual void write( const record& r );
private:	
	tcode::io::ip::udp_base _socket;
	tcode::io::ip::address _endpoint;	
	tcode::buffer::byte_buffer _buffer;
};

}}}

#endif