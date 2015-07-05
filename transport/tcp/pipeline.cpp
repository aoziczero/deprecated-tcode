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

void pipeline::fire_filter_on_read( tcode::buffer::byte_buffer& buf ){
	if ( _inbound )
		_inbound->filter_on_read( buf );
}

void pipeline::fire_filter_on_write( int written , bool flush ){
	if ( _inbound )
		_inbound->filter_on_write( written , flush);
}

void pipeline::fire_filter_on_error( const tcode::diagnostics::error_code& ec ) {
	if ( _inbound ) 
		_inbound->filter_on_error( ec );
}

void pipeline::fire_filter_on_close( void ) {
	if ( _inbound ) 
		_inbound->filter_on_close();
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