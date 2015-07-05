#include "stdafx.h"
#include "completion_handler_connect.hpp"
#include "channel.hpp"
#include <diagnostics/windows_category_impl.hpp>

namespace tcode { namespace transport { namespace tcp {

completion_handler_connect::completion_handler_connect( tcode::transport::event_loop& l 
	, pipeline_builder* builder 
	, const tcode::time_span& ts )
	: _loop(l) , _builder(builder)
{
	add_ref();
	_timer = tcode::transport::event_timer::create( _loop );
	_timer->expired_at( tcode::time_stamp::now() + ts );
	_timer->on_expired( 
		tcode::transport::event_timer::handler(
			[this] ( const tcode::diagnostics::error_code& ec , const event_timer& )
			{
				on_timer();
				release();
			}));
	_timer->fire();
}

completion_handler_connect::~completion_handler_connect( void ){

}	

#if defined( TCODE_TARGET_WINDOWS)
void completion_handler_connect::operator()( const tcode::diagnostics::error_code& ec 
		, const int completion_bytes )
{
	
}
#elif defined( TCODE_TARGET_LINUX )
void completion_handler_connect::operator()( const int events )
{
	
}
#endif


}}}

