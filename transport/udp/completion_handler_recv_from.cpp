#include "stdafx.h"
#include "completion_handler_recv_from.hpp"
#include "channel.hpp"
#include <diagnostics/windows_category_impl.hpp>

namespace tcode { namespace transport { namespace udp {

completion_handler_recv_from::completion_handler_recv_from( channel& channel )
	: _channel(channel){
	_channel.add_ref();
}

completion_handler_recv_from::~completion_handler_recv_from( void ){
	_channel.release();
}
	
void completion_handler_recv_from::operator()( const tcode::diagnostics::error_code& ec 
		, const int completion_bytes )
{
	tcode::diagnostics::error_code errc = ec;
	if ( completion_bytes == 0 && !errc ) {
		errc = tcode::diagnostics::error_code( WSAECONNRESET , tcode::diagnostics::windows_category());
	} 
	if ( !errc) {
		_read_buffer.wr_ptr( completion_bytes );
	}
	_channel.handle_recv_from( errc ,  _read_buffer , _address , this );
}

tcode::iovec completion_handler_recv_from::read_buffer( const int sz ){
	_read_buffer.clear();
	_read_buffer.reserve( sz );
	return tcode::io::read_buffer( _read_buffer );
}

tcode::io::ip::address& completion_handler_recv_from::address( void ){
	return _address;
}

}}}

