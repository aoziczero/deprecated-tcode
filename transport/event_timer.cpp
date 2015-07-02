#include "stdafx.h"
#include "event_timer.hpp"
#include <threading/atomic.hpp>
#include "event_loop.hpp"

namespace tcode { namespace transport {
namespace {

int FLAG_CANCEL = 0x01;

}

event_timer::event_timer( event_loop& l )	
	: _loop( l )
{
	_expired_at = tcode::time_stamp::now();
}

event_timer::~event_timer( void ){
	
}
	
tcode::time_stamp event_timer::expired_at( void ) const{
	return _expired_at;
}

void event_timer::expired_at( const tcode::time_stamp& at ) {
	_expired_at = at;
}

event_timer::handler event_timer::on_expired( void ) const {
	return _handler;
}
	
void event_timer::on_expired( const event_timer::handler& h )  {
	_handler = h;
}

void event_timer::operator()(void){
	tcode::diagnostics::error_code ec;
	if ( tcode::threading::atomic_bit_on( _flag , FLAG_CANCEL )){
		ec = tcode::diagnostics::cancel;
	}
	_handler( ec , *this);
}

void event_timer::fire( void ){
	tcode::rc_ptr< event_timer > ptr( this );
	if ( _loop.in_event_loop() ) {
		_loop.schedule( ptr );
	} else {
		_loop.execute( [ptr]{
			ptr->_loop.schedule( ptr );
		});
	}
}

void event_timer::cancel( void ){
	if ( tcode::threading::atomic_bit_set( _flag , FLAG_CANCEL )) {
		tcode::rc_ptr< event_timer > ptr( this );
		if ( _loop.in_event_loop() ) {
			ptr->_loop.cancel( ptr );
		} else {
			_loop.execute( [ptr]{
				ptr->_loop.cancel( ptr );
			});
		}
	}
}

event_timer::pointer_type event_timer::create( event_loop& l){
	return rc_ptr< event_timer >( new event_timer(l));
}

}}
