#include "stdafx.h"
#include "filter.hpp"
#include <transport/event_loop.hpp>
#include "channel.hpp"
#include "pipeline.hpp"

namespace tcode { namespace transport { namespace tcp {

filter::filter( void )
	: _pipeline(nullptr) 
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
	//fire_filter_on_end_reference();
	delete this;
}
	
// outbound
void filter::filter_do_write( tcode::buffer::byte_buffer buf ){
	fire_filter_do_write(buf);
}	

// fire inbound
void filter::fire_filter_on_open( const tcode::io::ip::address& addr ){
	if ( _inbound )
		pipeline()->fire_filter_on_open( _inbound , addr );
}

void filter::fire_filter_on_close( void ){
	if ( _inbound )
		pipeline()->fire_filter_on_close( _inbound  );
}

void filter::fire_filter_on_read( tcode::buffer::byte_buffer& buf ){
	if ( _inbound )
		pipeline()->fire_filter_on_read( _inbound , buf );
		
}
void filter::fire_filter_on_write( int written , bool flush ){
	if ( _inbound )
		pipeline()->fire_filter_on_write( _inbound , written , flush );
}

void filter::fire_filter_on_error( const std::error_code& ec ){
	if ( _inbound )
		pipeline()->fire_filter_on_error( _inbound , ec );	
}

void filter::fire_filter_do_write( tcode::buffer::byte_buffer& buf ){
	pipeline()->fire_filter_do_write( _outbound , buf );
}

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
	return _pipeline->channel();
}

tcp::pipeline* filter::pipeline( void ){
	return _pipeline;
}

void filter::pipeline( tcp::pipeline* p ){
	_pipeline = p;
}

void filter::add_ref( void ) {
	channel()->add_ref();
}

int filter::release( void ){
	return channel()->release();
}

void filter::close( const tcode::diagnostics::error_code& ec ){
	channel()->close( ec );
}

}}}
