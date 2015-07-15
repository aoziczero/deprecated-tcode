#ifndef __tcode_transport_tcp_packet_buffer_h__
#define __tcode_transport_tcp_packet_buffer_h__


#include <common/rc_ptr.hpp>
#include <buffer/byte_buffer.hpp>
#include <io/io.hpp>

namespace tcode{ namespace transport { namespace tcp{ 

class packet_buffer 
	: public tcode::ref_counted<packet_buffer>
{
public:
	packet_buffer( void );
	virtual ~packet_buffer( void );
	
	virtual tcode::iovec read_iovec(void) = 0;
	virtual bool complete( size_t nread ) = 0;
	virtual tcode::buffer::byte_buffer detach_packet( void ) = 0;
};

class simple_packet_buffer 
	: public packet_buffer
{
public:
	simple_packet_buffer( size_t nbyte );
	virtual ~simple_packet_buffer( void );
	
	virtual tcode::iovec read_iovec(void);
	virtual bool complete( size_t nread );
	virtual tcode::buffer::byte_buffer detach_packet( void );
private:
	tcode::buffer::byte_buffer _buffer;
	size_t _buffer_size;
};

typedef tcode::rc_ptr<packet_buffer> packet_buffer_ptr;

}}}

#endif