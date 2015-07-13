#include "stdafx.h"
#include "size_filter.hpp"
#include "channel.hpp"

namespace tcode { namespace transport { namespace tcp {

typedef int size_type;

size_filter::size_filter( void ){
}

size_filter::~size_filter( void ){
}

void size_filter::filter_on_read( tcode::buffer::byte_buffer buf )
{
	_read_buffer.reserve( _read_buffer.length() + buf.length());
	_read_buffer.write( buf.rd_ptr() , buf.length());
	
	while ( _read_buffer.length() > sizeof( size_type ) ){
		size_type size;
		_read_buffer.peak( &size , sizeof(size_type));
		if (  _read_buffer.length() < sizeof( size ) + size ){
			break;
		}
		_read_buffer.read( &size , sizeof(size_type));
		fire_filter_on_read( _read_buffer.sub_buffer( 0 , size));
	}
	_read_buffer.fit();
}

void size_filter::filter_do_write( tcode::buffer::byte_buffer buf )
{	
	if ( outbound()) {
		tcode::buffer::byte_buffer buff( buf.length() + sizeof(size_type) );
		buff << (size_type)buf.length();
		buff.write( buf.rd_ptr() , buf.length());
		fire_filter_do_write(buff);
	} else {
		tcode::buffer::byte_buffer size(sizeof(size_type));
		size << (size_type)buf.length();
		channel()->do_write( size , buf );
	}
}

}}}