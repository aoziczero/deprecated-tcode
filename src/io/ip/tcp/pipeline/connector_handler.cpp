#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/connector_handler.hpp>
#include <tcode/time/timestamp.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

connector_handler::connector_handler( io::engine& e )
    : _engine( e )
	, _current_address_index(0)
	, _timeout( tcode::timespan::minutes(1))
	, _connect_in_progress(false)
    , _socket(engine())
    , _timer(engine())
{
	
}

connector_handler::~connector_handler( void ){
	
}

io::engine& connector_handler::engine( void ) {
    return _engine;
}

tcode::timespan connector_handler::timeout( void ){
	return _timeout;
}

void connector_handler::timeout( const tcode::timespan& timeout ){
	_timeout = timeout;
}

void connector_handler::do_connect(const std::vector< tcode::io::ip::address >& addrs ){	
	_address = addrs;
	_current_address_index = 0;
    _timer.due_time( _timeout )
        .repeat( _timeout )
        .callback( [this]( const std::error_code& ec ) {
                    on_timer(ec);
                });
	do_connect();
    _timer.fire();
}

void connector_handler::do_connect(){
	tcode::io::ip::address& addr = _address[_current_address_index];	
    _connect_in_progress = true;
    _connect_time = tcode::timestamp::now();
    _socket.connect( addr , [this]( const std::error_code& ec 
                , const tcode::io::ip::address& addr )
            {
                handle_connect(ec);          
            });
}

void connector_handler::handle_connect( const std::error_code& ec  ){
	_connect_in_progress = false;
	std::error_code er = ec;
	if ( !er ) {
        /*
		tcode::io::ip::tcp::channel* channel = new tcode::io::ip::tcp::channel( 
						std::move( _socket ));
		if ( build( channel->pipeline() ) ) {
			channel->fire_on_open(_address[_current_address_index]);
			_timer->cancel();
			return;
		} 
		delete channel;
        */
		er = tcode::error_pipeline_build_fail;
    }
    _socket.close();
	connector_on_error(ec , _address[_current_address_index] );
	_current_address_index = ( _current_address_index + 1 ) % _address.size();
	if (connector_do_continue(_address[_current_address_index])){		
		do_connect();
	} else {
		_timer.cancel();
	}
}

void connector_handler::on_timer( const std::error_code& ec ){
    if( ec ) return;
	if ( _connect_in_progress ) {
		if ( tcode::timestamp::now() > _connect_time + _timeout ){
            _socket.close();
        }
	} 
}

}}}}

