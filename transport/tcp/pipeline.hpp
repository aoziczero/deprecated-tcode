#ifndef __tcode_transport_tcp_pipeline_h__
#define __tcode_transport_tcp_pipeline_h__

#include <thread>
#include <buffer/byte_buffer.hpp>
#include <io/ip/address.hpp>

namespace tcode { namespace transport { namespace tcp {

class channel;
class filter;
class pipeline{
public:
	pipeline( void );
	pipeline( const pipeline& pl );
	~pipeline( void );
	
	bool in_pipeline( void );
	void fire_filter_on_open( const tcode::io::ip::address& addr );
	void fire_filter_on_read( tcode::buffer::byte_buffer& buf );
	void fire_filter_on_write( int written , bool flush );
	void fire_filter_on_error( const tcode::diagnostics::error_code& ec );
	void fire_filter_on_close( void );
	void fire_filter_on_end_reference( void );

	void fire_filter_on_open(  filter* pfilter , const tcode::io::ip::address& addr );
	void fire_filter_on_close( filter* pfilter );
	void fire_filter_on_read( filter* pfilter , tcode::buffer::byte_buffer& buf );
	void fire_filter_on_write( filter* pfilter , int written , bool flush );
	void fire_filter_on_error( filter* pfilter , const std::error_code& ec );
	void fire_filter_do_write( filter* pfilter , tcode::buffer::byte_buffer& buf );

	pipeline& add( filter* pfilter );
	
	void channel( tcp::channel* chan );
	tcp::channel* channel( void );
private:
	tcp::channel* _channel;
	filter* _inbound;
	int _flag;	
};

}}}

#endif