#ifndef __tcode_io_ip_tcp_timeout_filter_h__
#define __tcode_io_ip_tcp_timeout_filter_h__

#include <tcode/time/timestamp.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/byte_buffer.hpp>
#include <tcode/io/engine.hpp>
#include <tcode/io/ip/tcp/pipeline/filter.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

class timeout_filter : public filter{
public:
	timeout_filter( const tcode::timespan& timeout );
	virtual ~timeout_filter( void );
	virtual void filter_on_open( const tcode::io::ip::address& addr );
	virtual void filter_on_read( tcode::byte_buffer buf );
	virtual void filter_on_close( void );
	void on_timer( const std::error_code& ec );
private:
	tcode::timestamp _stamp;
	tcode::timespan _timeout;
    tcode::io::timer* _timer;
};

}}}}

#endif
