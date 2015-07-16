#include "stdafx.h"
#include "acceptor_handler.hpp"


namespace tcode { namespace transport { namespace tcp {

acceptor_handler::acceptor_handler( void ){
	
}

acceptor_handler::~acceptor_handler( void ){
	
}

bool acceptor_handler::condition( tcode::io::ip::socket_type h 
		, const tcode::io::ip::address& addr )
{
	return true;
}

void acceptor_handler::acceptor_on_error( const tcode::diagnostics::error_code& ec ){
}

}}}

