#ifndef __tcode_transport_udp_pipeline_h__
#define __tcode_transport_udp_pipeline_h__

#include <thread>
#include <buffer/byte_buffer.hpp>
#include <io/ip/address.hpp>

namespace tcode { namespace transport { namespace udp {

class channel;
class filter;
class pipeline{
public:
	pipeline( void );
	pipeline( const pipeline& pl );
	~pipeline( void );
	
	bool in_pipeline( void );
	void fire_filter_on_open( );
	void fire_filter_on_read(tcode::buffer::byte_buffer buf 
		,const tcode::io::ip::address& addr );
	void fire_filter_on_error( const tcode::diagnostics::error_code& ec );
	void fire_filter_on_close( void );
	void fire_filter_on_end_reference( void );

	pipeline& add( filter* pfilter );
	
	void set_channel( udp::channel* chan );
private:
	int _flag;	
	filter* _inbound;
};

}}}

#endif