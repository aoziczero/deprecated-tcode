#include "stdafx.h"
#include "connector.hpp"

#include "pipeline.hpp"
#include "pipeline_builder.hpp"
#include "channel.hpp"
#include "completion_handler_connect.hpp"

namespace tcode { namespace transport { namespace tcp {

connector::connector(tcode::transport::event_loop& l)
	: _loop( l )
{

}

connector::~connector(void) {
}

bool connector::connect_timeout( const tcode::io::ip::address& conn_addr
		, pipeline_builder* builder 
		, const tcode::time_span& ts )
{
	tcode::io::ip::connect_base cb;
	if (cb.open( conn_addr.family())){
		tcode::io::ip::connect_base::non_blocking non_block;
		cb.set_option( non_block );
		if ( cb.connect( conn_addr , ts )){
			auto handle = cb.handle();
			_loop.execute( [handle,conn_addr,builder]{
				tcode::transport::tcp::pipeline pl;
				if ( builder && builder->build( pl ) ) {
					tcode::transport::tcp::channel* channel 
							= new tcode::transport::tcp::channel( 
									builder->channel_loop() , pl ,handle);
					channel->fire_on_open( conn_addr );
				} else {
					tcode::io::ip::tcp_holder h;
					h.handle( handle );
					h.close();
				}
			});
		}
	}
	cb.close();
	return false;
}

connect_id connector::connect(const tcode::io::ip::address& conn_addr
		, pipeline_builder* builder 
		, const tcode::time_span& ts )
{
	connect_id ci( new completion_handler_connect(_loop , builder ,ts));
	return ci;
}

}}}
