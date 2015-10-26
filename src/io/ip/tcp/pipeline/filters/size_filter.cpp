#include "stdafx.h"
#include <tcode/io/ip/tcp/pipeline/filters/size_filter.hpp>
#include <tcode/io/ip/tcp/pipeline/channel.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

typedef int size_type;

size_filter::size_filter( void ){
}

size_filter::~size_filter( void ){
}

void size_filter::filter_on_read( tcode::byte_buffer buf )
{
	_read_buffer.reserve( static_cast<int>(_read_buffer.length() + buf.length()));
	_read_buffer.write( buf.rd_ptr() , buf.length());
	
	while ( _read_buffer.length() > sizeof( size_type ) ){
		size_type size;
		_read_buffer.peak( &size , sizeof(size_type));
		if (  _read_buffer.length() < sizeof( size ) + size ){
			break;
		}
		_read_buffer.read( &size , sizeof(size_type));
		auto packet = _read_buffer.sub_buffer( 0 , size);
		fire_filter_on_read(packet);
	}
	_read_buffer.fit();
}

void size_filter::filter_do_write( tcode::byte_buffer buf )
{	
	if ( !outbound() && pipeline()->in_pipeline() ){
		tcode::byte_buffer size(sizeof(size_type));
		size << (size_type)buf.length();
		channel()->do_write( size );
		channel()->do_write( buf );
	}else {
		tcode::byte_buffer buff( buf.length() + sizeof(size_type) );
		buff << (size_type)buf.length();
		buff.write( buf.rd_ptr() , buf.length());
		fire_filter_do_write(buff);
	}
}

}}}}
