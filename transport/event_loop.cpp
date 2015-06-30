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


}}
