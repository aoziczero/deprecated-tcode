	#include "stdafx.h"
#include "timeout_filter.hpp"
#include <transport/event_timer.hpp>
#include <transport/tcp/channel.hpp>
namespace tcode { namespace transport { namespace tcp {

timeout_filter::timeout_filter( const tcode::time_span& timeout )
	: _timeout( timeout ) , _stamp( tcode::time_stamp::now() )
	, _closed(false)
{
	
}

timeout_filter::~timeout_filter( void ) {

}

void timeout_filter::filter_on_open( const tcode::io::ip::address& addr ){
	_stamp = tcode::time_stamp::now();
	add_ref();
	tcode::transport::event_timer::pointer_type et = 
		tcode::transport::event_timer::create( channel()->loop());
	et->expired_at( _stamp + _timeout );
	et->on_expired( 
		tcode::transport::event_timer::handler(
			[this]( const tcode::diagnostics::error_code& ec , const tcode::transport::event_timer& )	{
				on_timer();
				release();
			}
		));
	et->fire();
}

void timeout_filter::on_timer( void ){
	if ( _closed ) {
		return;
	}	
	if ( ( tcode::time_stamp::now() - _stamp ) > _timeout ){
		close( tcode::diagnostics::timeout );
		return;
	}
	add_ref();
	tcode::transport::event_timer::pointer_type et = 
		tcode::transport::event_timer::create( channel()->loop());
	et->expired_at( _stamp + _timeout );
	et->on_expired( 
		tcode::transport::event_timer::handler(
			[this]( const tcode::diagnostics::error_code& ec , const tcode::transport::event_timer& )	{
				on_timer();
				release();
			}
		));
	et->fire();	
}

void timeout_filter::filter_on_read( tcode::buffer::byte_buffer buf ){
	_stamp = tcode::time_stamp::now();
	fire_filter_on_read(buf);
}

void timeout_filter::filter_on_close( void ){
	_closed = true;
	fire_filter_on_close();
}

}}}