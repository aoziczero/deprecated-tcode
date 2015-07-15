#include "stdafx.h"
#include "packet_buffer.hpp"

namespace tcode{ namespace transport { namespace tcp{ 

packet_buffer::packet_buffer( void ){
}

packet_buffer::~packet_buffer( void ){
}

simple_packet_buffer::simple_packet_buffer( size_t nbyte )
	: _buffer_size(nbyte)
	, _buffer(nbyte)
{

}

simple_packet_buffer::~simple_packet_buffer( void ){

}
	
tcode::iovec simple_packet_buffer::read_iovec( void ) {
	return tcode::io::read_buffer( _buffer );
}


bool simple_packet_buffer::complete( size_t nread  ){
	_buffer.wr_ptr( nread );
	return true;
}

tcode::buffer::byte_buffer simple_packet_buffer::detach_packet( void ){
	tcode::buffer::byte_buffer buf(_buffer_size);
	_buffer.swap( buf );
	return buf;
}


}}}