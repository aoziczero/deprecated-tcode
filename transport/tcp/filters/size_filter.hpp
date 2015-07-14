#ifndef __tcode_transport_tcp_size_filter_h__
#define __tcode_transport_tcp_size_filter_h__

#include <buffer/byte_buffer.hpp>
#include <io/ip/address.hpp>
#include <diagnostics/tcode_error_code.hpp>
#include <transport/tcp/filter.hpp>

namespace tcode { namespace transport { namespace tcp {

class size_filter : public filter{
public:
	size_filter( void );
	virtual ~size_filter( void );
	virtual void filter_on_read( tcode::buffer::byte_buffer buf );
	virtual void filter_do_write( tcode::buffer::byte_buffer buf );	
private:
	tcode::buffer::byte_buffer _read_buffer;	
};

}}}

#endif