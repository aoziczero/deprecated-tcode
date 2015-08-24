#include "stdafx.h"
#include "bootstrap.hpp"
#include <io/ip/basic_socket.hpp>

namespace tcode { namespace transport { namespace udp {

bool bootstrap::open_server( const tcode::io::ip::address& addr
		, tcode::transport::udp::pipeline_builder_ptr& builder )
{
	tcode::io::ip::udp_base fd;
	if (fd.open( addr.family())) {
		tcode::io::ip::udp_base::non_blocking nb;
		fd.set_option(nb);
		if ( fd.bind( addr )){
			tcode::transport::udp::pipeline pl;
			if ( builder && builder->build(pl)){
				tcode::transport::udp::channel* channel
					= new tcode::transport::udp::channel(
						builder->channel_loop() , pl , fd.handle());
				channel->fire_on_open();
				return true;
			}
		}
	}
	fd.close();
	return false;
}

bool bootstrap::open_client( int af 
		, tcode::transport::udp::pipeline_builder_ptr& builder )
{
	tcode::io::ip::udp_base fd;
	if (fd.open( af )) {
		tcode::io::ip::udp_base::non_blocking nb;
		fd.set_option(nb);
		tcode::transport::udp::pipeline pl;
		if ( builder && builder->build(pl)){
			tcode::transport::udp::channel* channel
				= new tcode::transport::udp::channel(
					builder->channel_loop() , pl , fd.handle());
			channel->fire_on_open();
			return true;
		}
	}
	fd.close();
	return false;
}
}}}