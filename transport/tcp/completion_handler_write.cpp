#include "stdafx.h"
#include "completion_handler_write.hpp"
#include "channel.hpp"
#include <diagnostics/windows_category_impl.hpp>

namespace tcode { namespace transport { namespace tcp {

completion_handler_write::completion_handler_write( channel& channel )
	: _channel(channel)
{
	_channel.add_ref();
}

completion_handler_write::~completion_handler_write( void ){
	_channel.release();
}
	
void completion_handler_write::operator()( const tcode::diagnostics::error_code& ec 
		, const int completion_bytes )
{
	tcode::diagnostics::error_code errc = ec;
	if ( completion_bytes == 0 && !errc )
		errc = tcode::diagnostics::error_code( WSAECONNRESET , tcode::diagnostics::windows_category());
	_channel.handle_write( errc , completion_bytes , this );
}

}}}

