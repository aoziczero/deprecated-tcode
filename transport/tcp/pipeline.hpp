#ifndef __tcode_transport_tcp_pipeline_h__
#define __tcode_transport_tcp_pipeline_h__

#include <thread>
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
	void fire_filter_on_end_reference( void );

	pipeline& add( filter* pfilter );
	
	void set_channel( tcp::channel* chan );
private:
	int _flag;	
	filter* _inbound;
};

}}}

#endif