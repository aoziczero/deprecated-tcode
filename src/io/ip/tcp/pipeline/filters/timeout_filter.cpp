#include "stdafx.h"
#include <tcode/error.hpp>
#include <tcode/io/ip/tcp/pipeline/filters/timeout_filter.hpp>
#include <tcode/io/ip/tcp/pipeline/channel.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

timeout_filter::timeout_filter( const tcode::timespan& timeout )
	: _timeout( timeout )
    , _stamp( tcode::timestamp::now() )
    , _timer( nullptr )
{
}

timeout_filter::~timeout_filter( void ) {
}

void timeout_filter::filter_on_open( const tcode::io::ip::address& addr ){
	_stamp = tcode::timestamp::now();	
    _timer = new tcode::io::timer( channel()->engine() );
    _timer->due_time( _stamp + _timeout )
        .repeat( _timeout )
        .callback( [this]( const std::error_code& ec ) {
                    on_timer( ec );
                });
	_timer->fire();
    add_ref();
}

void timeout_filter::on_timer( const std::error_code& ec ){
	if ( ec ) {
        release();
    } else {
        if ( tcode::timestamp::now() - _stamp > _timeout ) {
            close( tcode::error_timeout );
        }
    }
}

void timeout_filter::filter_on_read( tcode::byte_buffer buf ){
	_stamp = tcode::timestamp::now();
	fire_filter_on_read(buf);
}

void timeout_filter::filter_on_close( void ){
    _timer->cancel();
	fire_filter_on_close();
}

}}}}
