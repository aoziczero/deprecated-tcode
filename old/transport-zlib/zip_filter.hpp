#ifndef __tcode_zlib_filter_h__
#define __tcode_zlib_filter_h__

#include <transport/tcp/filter.hpp>

namespace tcode { namespace transport { namespace tcp {

class zip_filter 
	: public tcode::transport::tcp::filter {
public:
	zip_filter( void );
	virtual ~zip_filter( void );

	virtual void filter_on_read( tcode::buffer::byte_buffer buf );
	virtual void filter_do_write( tcode::buffer::byte_buffer buf );	
private:
	tcode::buffer::byte_buffer _read_buffer;
};

}}}

#endif