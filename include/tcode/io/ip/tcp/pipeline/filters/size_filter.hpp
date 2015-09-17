#ifndef __tcode_io_ip_tcp_size_filter_h__
#define __tcode_io_ip_tcp_size_filter_h__

#include <tcode/io/ip/tcp/pipeline/filter.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

class size_filter : public filter{
public:
	size_filter( void );
	virtual ~size_filter( void );
	virtual void filter_on_read( tcode::byte_buffer buf );
	virtual void filter_do_write( tcode::byte_buffer buf );	
private:
	tcode::byte_buffer _read_buffer;	
};

}}}}

#endif
