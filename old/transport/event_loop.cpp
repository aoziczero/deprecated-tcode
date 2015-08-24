#include "stdafx.h"
#include "event_loop.hpp"
namespace tcode { namespace transport {

event_loop::event_loop( void ){
	_active_links.store(0);
}

event_loop::~event_loop( void ) {

}

void event_loop::links_add( void ) {
	_active_links.fetch_add(1);
}

void event_loop::links_release( void ){
	_active_links.fetch_sub(1);
}

transport::dispatcher& event_loop::dispatcher( void ) {
	return _dispatcher;
}

void event_loop::run( void ){
	_thread_id = std::this_thread::get_id();
	while ( _active_links.load() || !_event_timers.empty() ) {
		_dispatcher.run( wake_up_time() );
		schedule_timer();
	}
}

bool event_loop::in_event_loop( void ){
	return std::this_thread::get_id() == _thread_id;
}

void event_loop::execute_handler( tcode::io::completion_handler* handler ){
	_dispatcher.post( handler );
}

void event_loop::schedule( const event_timer_ptr& ptr ) {
	std::list<event_timer_ptr>::iterator it = 
        std::upper_bound( _event_timers.begin() , _event_timers.end() , ptr , 
			[]( const event_timer_ptr& v , const event_timer_ptr& cmp )->bool{
                return v->expired_at() < cmp->expired_at();
            });
    _event_timers.insert( it , ptr );
}

void event_loop::cancel( const event_timer_ptr& ptr ){
	auto it = std::find(_event_timers.begin(), _event_timers.end() , ptr );
	if (it != _event_timers.end()){
		_event_timers.erase( it );
		(*ptr)();
	}
}

tcode::time_span event_loop::wake_up_time( void ){
	if( _event_timers.empty() ) {
		return tcode::time_span::days(1);
	}
	tcode::time_stamp now = tcode::time_stamp::now();
	if ( _event_timers.front()->expired_at() <= now ) {
		return tcode::time_span::seconds(0);
	}
	return _event_timers.front()->expired_at() - now;
}

void event_loop::schedule_timer( void ){
	tcode::time_stamp ts = tcode::time_stamp::now();
	while( !_event_timers.empty() ) {
		event_timer_ptr ptr = _event_timers.front();
		if (  ptr->expired_at() <= ts ){
			_event_timers.pop_front();
			(*ptr)();
		}else {
			break;
		}
	}
}

}}
