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
	delete this;
}
	
// outbound
void filter::filter_do_write( tcode::buffer::byte_buffer buf ){
	fire_filter_do_write(buf);
}	

// fire inbound
void filter::fire_filter_on_open( const tcode::io::ip::address& addr ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		if ( _inbound )
			_inbound->filter_on_open( addr );
	} else {
		add_ref();
		channel()->loop().execute( [ this , addr ] {
			if ( _inbound )
				_inbound->filter_on_open( addr );
			release();
		});
	}	
}
void filter::fire_filter_on_close( void ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		if ( _inbound )
			 _inbound->filter_on_close( );
	} else {
		add_ref();
		channel()->loop().execute( [ this  ] {
			if ( _inbound )
				 _inbound->filter_on_close( );
			release();
		});
	}
}
void filter::fire_filter_on_read( tcode::buffer::byte_buffer& buf ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		if ( _inbound )
			_inbound->filter_on_read( buf );
	} else {
		add_ref();
		channel()->loop().execute( [ this , buf  ] {
			tcode::buffer::byte_buffer nb( buf );
			if ( _inbound )
				_inbound->filter_on_read( buf );
			release();
		});
	}	
}
void filter::fire_filter_on_write( int written , bool flush ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		if ( _inbound )
			 _inbound->filter_on_write( written , flush );
	} else {
		add_ref();
		channel()->loop().execute( [ this , written , flush ] {
			if ( _inbound )
				_inbound->filter_on_write( written , flush );
			release();
		});
	}	
}
void filter::fire_filter_on_error( const std::error_code& ec ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		if ( _inbound )
				_inbound->filter_on_error( ec );
	} else {
		add_ref();
		channel()->loop().execute( [ this , ec  ] {			
			if ( _inbound )
				 _inbound->filter_on_error( ec );
			release();
		});
	}
}
void filter::fire_filter_on_end_reference( void ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		if ( _inbound )
			_inbound->filter_on_end_reference();
	} else {
		add_ref();
		channel()->loop().execute( [ this ] {
			if ( _inbound )
				_inbound->filter_on_end_reference();
			release();
		});
	}	
}	
void filter::fire_filter_do_write( tcode::buffer::byte_buffer& buf ){
	if ( channel()->loop().in_event_loop() && channel()->pipeline().in_pipeline() ){
		if ( _outbound )
			_outbound->filter_do_write( buf );
		else 
			_channel->do_write( buf );
	} else {
		add_ref();
		channel()->loop().execute( [ this , buf  ] {
			tcode::buffer::byte_buffer nb( buf );
			if ( _outbound )
				_outbound->filter_do_write( buf );
			else 
				_channel->do_write( buf );
			release();
		});
	}
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

void filter::close( const tcode::diagnostics::error_code& ec ){
	_channel->close( ec );
}

}}}
