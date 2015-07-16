#include "stdafx.h"
#include "connector_handler.hpp"
#include "completion_handler_connect.hpp"
#include "pipeline.hpp"
#include "channel.hpp"

namespace tcode { namespace transport { namespace tcp {

connector_handler::connector_handler( void )
	: _current_address(0)
{
	
}

connector_handler::~connector_handler( void ){
	
}

void connector_handler::connector_on_error( const tcode::diagnostics::error_code& ec ){
	//
}

tcode::time_span connector_handler::timeout( void ){
	return _timeout;
}

void connector_handler::timeout( const tcode::time_span& timeout ){
	_timeout = timeout;
}

bool connector_handler::do_connect(const std::vector< tcode::io::ip::address >& addrs ){	
	connector_handler_ptr ptr(this);
	_timer->on_expired( 
		[ptr]( const tcode::diagnostics::error_code& ec , tcode::transport::event_timer& )
		{
			ptr->on_timer(ec);
		});
}

void connector_handler::handle_connect( const tcode::diagnostics::error_code& ec  ){
	if ( !ec ) {
		tcode::transport::tcp::pipeline pl;
		if ( build( pl ) ) {
			tcode::transport::tcp::channel* channel 
				= new tcode::transport::tcp::channel( 
						*_channel_loop ,  pl , handle( tcode::io::ip::invalid_socket ));
			channel->fire_on_open(_address[_current_address]);
			return;
		} else {
			er = tcode::diagnostics::build_fail;
		}
	}
	_builder->connector_on_error(er);
	tcode::io::ip::connect_base::close();
	this->release();
}

void connector_handler::on_timer( const tcode::diagnostics::error_code& ec ){
	if ( ec == tcode::diagnostics::cancel ) {
		_timer.reset();
	} else {
		if ( _connect_time + _timeout > tcode::time_stamp::now() ){
			close();
#if defined( TCODE_TARGET_WINDOWS )
			_completion_handler->cancel();
#endif
			do_connect();
		}
	}
}

}}}

