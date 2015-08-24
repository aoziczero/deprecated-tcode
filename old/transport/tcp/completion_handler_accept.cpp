#include "stdafx.h"
#include "completion_handler_accept.hpp"
#include <transport/event_loop.hpp>
#include <diagnostics/windows_category_impl.hpp>
#include <threading/atomic.hpp>
#include "acceptor.hpp"

namespace tcode { namespace transport { namespace tcp {
completion_handler_accept::completion_handler_accept( acceptor& pacceptor )
	: _acceptor(pacceptor){
	_acceptor.loop().links_add();
}

completion_handler_accept::~completion_handler_accept( void ){
	_acceptor.loop().links_release();
}
	
void completion_handler_accept::operator()( const tcode::diagnostics::error_code& ec 
		, const int completion_bytes )
{
	_acceptor.handle_accept( ec , this );
}

SOCKET completion_handler_accept::handle( void ){
	return _handle;
}

void completion_handler_accept::handle( SOCKET s ){
	_handle = s;
}
		
tcode::io::ip::address* completion_handler_accept::address_ptr( void ) {
	return _address;
}

}}}

