#include "stdafx.h"
#include "udp_writer.hpp"
#include "record.hpp"

namespace tcode { namespace diagnostics { namespace log {

udp_writer::udp_writer( const tcode::io::ip::address& endpoint )
	: _endpoint(endpoint)
{
	
}

udp_writer::~udp_writer( void ){

}

void udp_writer::write( const record& r ){
	if ( !_socket.good() ) {
		_socket.open( AF_INET );
	}	
	if ( _socket.good() ) {
		formatter()->format( r , _buffer );	
		_socket.sendto( _buffer.rd_ptr() , _buffer.length() , _endpoint );
		_buffer.clear();
	}
}

}}}
