#include "stdafx.h"
#include "channel.hpp"
#include "pipeline.hpp"
#include <transport/event_loop.hpp>

namespace tcode{ namespace transport { 
namespace tcp{ 

channel::channel( event_loop& l
				, const tcp::pipeline& p 
				, tcode::io::ip::socket_type fd  )
	: _loop( l )
	, _pipeline(p)
{
	handle( fd );	
	_pipeline.set_channel( this );
}

channel::~channel( void ) {

}

event_loop& channel::loop( void ){
	return _loop;
}


tcp::pipeline& channel::pipeline( void ){
	return _pipeline;
}

void channel::add_ref( void ){
	_flag.fetch_add(1);
}

void channel::release( void ){
	int val = _flag.fetch_sub(1
			, std::memory_order::memory_order_release ) - 1;
	if ( val == 0 ) {
		_loop.execute([this]{
			fire_on_end_reference();
		});
	}
}

void channel::fire_on_open( const tcode::io::ip::address& addr ){
	add_ref();
	if ( _loop.in_event_loop() ){
		_loop.dispatcher().bind( reinterpret_cast<HANDLE>(handle()));
		_pipeline.fire_filter_on_open(addr);
		//_read();	
	} else {
		_loop.execute([this,addr]{
			fire_on_open(addr);
		});
	}
}

void channel::fire_on_end_reference( void ){
	_pipeline.fire_filter_on_end_reference();
	delete this;
}

}}}