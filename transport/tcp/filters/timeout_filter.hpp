#ifndef __tcode_transport_tcp_timeout_filter_h__
#define __tcode_transport_tcp_timeout_filter_h__

#include <common/time_stamp.hpp>
#include <buffer/byte_buffer.hpp>
#include <io/ip/address.hpp>
#include <diagnostics/tcode_error_code.hpp>
#include <transport/tcp./filter.hpp>

namespace tcode { namespace transport { namespace tcp {

class timeout_filter : public filter{
public:
	timeout_filter( const tcode::time_span& timeout );
	virtual ~timeout_filter( void );
	virtual void filter_on_open( const tcode::io::ip::address& addr );
	virtual void filter_on_read( tcode::buffer::byte_buffer buf );
	virtual void filter_on_close( void );
	void on_timer( void );
private:
	tcode::time_stamp _stamp;
	tcode::time_span _timeout;
	bool _closed;
};

}}}

#endif