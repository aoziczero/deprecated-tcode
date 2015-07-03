#include "stdafx.h"
#include "pipeline.hpp"
#include "filter.hpp"

namespace tcode { namespace transport { namespace tcp {

static const int FLAG_IN_PIPELINE = 0x01;

pipeline::pipeline( void )
	: _flag(0)
	, _inbound( nullptr )
{
}

pipeline::pipeline( const pipeline& pl )
	: _flag( pl._flag ) , _inbound(pl._inbound) 
{
	
}

pipeline::~pipeline( void )
{

}
	
bool pipeline::in_pipeline( void ){
	return _flag & FLAG_IN_PIPELINE;
}

void pipeline::fire_filter_on_end_reference( void ){
	if ( _inbound )
		_inbound->filter_on_end_reference();
}

void pipeline::fire_filter_on_open( const tcode::io::ip::address& addr ){
	if ( _inbound )
		_inbound->filter_on_open( addr );
}

pipeline& pipeline::add( filter* pfilter ){
	if( _inbound == nullptr )
		_inbound = pfilter;
	else {
		filter* last = _inbound;

		while( last->inbound() != nullptr )
			last = last->inbound();
	
		last->inbound( pfilter );
		pfilter->inbound( nullptr );
		pfilter->outbound(last);
	}
	return *this;
}

void pipeline::set_channel( tcp::channel* chan ){
	filter* pfilter = _inbound;
	while( pfilter != nullptr ){
		pfilter->channel( chan );
		pfilter = pfilter->inbound();
	}	
}

}}}