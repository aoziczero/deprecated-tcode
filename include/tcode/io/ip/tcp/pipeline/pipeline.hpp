#ifndef __tcode_io_ip_tcp_pipeline_h__
#define __tcode_io_ip_tcp_pipeline_h__

#include <thread>
#include <tcode/byte_buffer.hpp>
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

class channel;
class filter;
class pipeline{
public:
	pipeline( void );
	pipeline( const pipeline& pl );
	~pipeline( void );
	
	bool in_pipeline( void );
	void fire_filter_on_open( const tcode::io::ip::address& addr );
	void fire_filter_on_read( tcode::byte_buffer& buf );
	void fire_filter_on_write( int written , bool flush );
	void fire_filter_on_error( const std::error_code& ec );
	void fire_filter_on_close( void );
	void fire_filter_on_end_reference( void );

	void fire_filter_on_open(  filter* pfilter , const tcode::io::ip::address& addr );
	void fire_filter_on_close( filter* pfilter );
	void fire_filter_on_read( filter* pfilter , tcode::byte_buffer& buf );
	void fire_filter_on_write( filter* pfilter , int written , bool flush );
	void fire_filter_on_error( filter* pfilter , const std::error_code& ec );
	void fire_filter_do_write( filter* pfilter , tcode::byte_buffer& buf );
	
	pipeline& add( filter* pfilter );
		
	// in_pipeline() 내에 서 호출할것
	pipeline& add_inbound( filter* base , filter* pfilter );
	pipeline& add_outbound( filter* base , filter* pfilter );
	pipeline& remove( filter* pfilter );
	
	void channel( tcode::io::ip::tcp::channel* chan );
    tcode::io::ip::tcp::channel* channel( void );
private:
    tcode::io::ip::tcp::channel* _channel;
	filter* _inbound;
	int _flag;	
};

}}}}

#endif
