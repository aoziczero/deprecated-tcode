#include "stdafx.h"
#include "epoll.hpp"

#include <unistd.h>
#include <sys/eventfd.h>

namespace tcode { namespace io {

static const int MAX_EVT = 256;

epoll::handler::handler( void ){}
epoll::handler::~handler( void ){}

epoll::epoll( void )
	: _drain_req(false) 
	, _post(*this)
{
	handle( epoll_create(MAX_EVT) );
	bind( _post.handle() , EPOLLIN  , &_post );
}
epoll::~epoll( void )
{
	close( handle(-1) );
}

int epoll::run( const tcode::time_span& ts ){	
	struct epoll_event events[MAX_EVT];
	int nofd  = epoll_wait( handle()
		, events
		, MAX_EVT
		, static_cast<int>(ts.total_milliseconds()));
	if ( nofd == -1 ) {
		return errno == EINTR ? 0 : -1;
	} else {
		for ( int i = 0 ; i < nofd; ++i ){
			epoll::handler* handler =
					static_cast< epoll::handler* >( events[i].data.ptr );
			if ( handler ) {
				(*handler)( events[i].events );
			}
		}
	}
	if ( _drain_req ){
		_post.drain();
		_drain_req = false;
	}
	return 0;
}

bool epoll::bind( int fd
			, const int events
			, handler* phandler )
{
	epoll_event evt;
	evt.events = events;
	evt.data.ptr = phandler;
	if ( epoll_ctl( handle()
				, EPOLL_CTL_MOD
				, fd
				, &evt) == 0 )
		return true;
	if ( errno == ENOENT )
		return epoll_ctl( handle()
				, EPOLL_CTL_ADD
				, fd
				, &evt) == 0;
	return false;
}

void epoll::unbind( int fd ){
	epoll_ctl( handle() , EPOLL_CTL_DEL , fd , nullptr );
}

void epoll::wake_up(){
	_post.set();
}

void epoll::post( tcode::io::completion_handler* op )
{
	_post.post( op );
}

void epoll::drain_req( void ) {
	_drain_req = true;
}


epoll::post_handler::post_handler( epoll& e )
	: _event_fd( eventfd( 0 , EFD_NONBLOCK )) , _epoll(e)
{

}

epoll::post_handler::~post_handler( void ) {

}

int epoll::post_handler::handle( void ) {
	return _event_fd;
}

void epoll::post_handler::operator()( const int events ) {
	_epoll.drain_req();	
}

void epoll::post_handler::set( void ) {
	eventfd_write( _event_fd , 1 );
}

void epoll::post_handler::post( tcode::io::completion_handler* op )
{
	tcode::threading::scoped_lock<> guard( _lock );
	_posted.add_tail(op);
	set();
}

void epoll::post_handler::drain( void ){
	tcode::slist::queue< tcode::io::completion_handler > drain;
	eventfd_t val = 0;
	eventfd_read( _event_fd , &val );
	do {
		tcode::threading::scoped_lock<> guard( _lock );
		drain.add_tail( std::move(_posted));
	}while(0);
	while( !drain.empty()){
		tcode::io::completion_handler* op = drain.front();
		drain.pop_front();
		(*op)( op->error_code() , 0 );
	}
}

}}
