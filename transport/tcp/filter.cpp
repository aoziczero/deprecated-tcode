#include "stdafx.h"
#include "filter.hpp"
#include <transport/event_loop.hpp>
#include "channel.hpp"
#include "pipeline.hpp"

namespace tcode { namespace transport { namespace tcp {

filter::filter( void )
	: _channel(nullptr) 
	, _inbound(nullptr)
	, _outbound( nullptr )
{
}

filter::~filter( void ){
	
}
	
// inbound
void filter::filter_on_open( const tcode::io::ip::address& addr ) {
	fire_filter_on_open(addr);
}

void filter::filter_on_close( void ) {
	fire_filter_on_close();
}

void filter::filter_on_read( tcode::buffer::byte_buffer buf ) {
	fire_filter_on_read( buf );
}

void filter::filter_on_write( int written , bool flush ) {
	fire_filter_on_write(written,flush);
}

void filter::filter_on_error( const std::error_code& ec ){
	fire_filter_on_error(ec);
}

void filter::filter_on_end_reference( void ){
	fire_filter_on_end_reference();
}
	
// outbound
void filter::filter_do_write( tcode::buffer::byte_buffer buf ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		fire_filter_do_write( buf );
	} else {
		add_ref();
		channel()->loop().execute( [ this , buf  ] {
			tcode::buffer::byte_buffer nb( buf );
			fire_filter_do_write( nb );
			release();
		});
	}
}	
/*
void filter::filter_do_writev( const std::vector< tcode::buffer::byte_buffer>& bufs ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		fire_filter_do_writev( bufs);
	} else {
		add_ref();
		channel()->loop().execute( [ this , bufs  ] {
			fire_filter_do_writev( bufs );
			release();
		});
	}
}
*/

// fire inbound
void filter::fire_filter_on_open( const tcode::io::ip::address& addr ){
	if ( _inbound )
		 _inbound->filter_on_open( addr );
}
void filter::fire_filter_on_close( void ){
	if ( _inbound )
		 _inbound->filter_on_close( );
}
void filter::fire_filter_on_read( tcode::buffer::byte_buffer& buf ){
	if ( _inbound )
		 _inbound->filter_on_read( buf );
}
void filter::fire_filter_on_write( int written , bool flush ){
	if ( _inbound )
		 _inbound->filter_on_write( written , flush );
}
void filter::fire_filter_on_error( const std::error_code& ec ){
	if ( _inbound )
		 _inbound->filter_on_error( ec );
}
void filter::fire_filter_on_end_reference( void ){
	if ( _inbound )
		 _inbound->filter_on_end_reference();
}	
void filter::fire_filter_do_write( tcode::buffer::byte_buffer& buf ){
	if ( _outbound )
		 _outbound->filter_do_write( buf );
	else 
		_channel->do_write( buf );
}
/*
void filter::fire_filter_do_writev( const std::vector< tcode::buffer::byte_buffer>& bufs ){
	if ( _outbound )
		 _outbound->filter_do_writev( bufs );
	else 
		_channel->do_writev( bufs );
}*/
	
filter* filter::inbound( void ){
	return _inbound;
}

filter* filter::outbound( void ){
	return _outbound;
}

void filter::inbound( filter* f ){
	_inbound = f;
}

void filter::outbound( filter* f ){
	_outbound = f;
}

tcp::channel* filter::channel( void ){
	return _channel;
}

void filter::channel( tcp::channel* c ){
	_channel = c;
}

void filter::add_ref( void ) {
	_channel->add_ref();
}

void filter::release( void ){
	_channel->release();
}

}}}
