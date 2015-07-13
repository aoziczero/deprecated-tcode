#include "stdafx.h"
#include "filter.hpp"
#include <zlib/include/zlib.h>

namespace tcode {
namespace zlib {
namespace {
#pragma pack(push, 1)   
struct header {
	uLongf len;
	uLongf orig_len;
};	
#pragma pack(pop) 
}

filter::filter( void ){
}
filter::~filter( void ){
}

void filter::filter_on_read( tcode::buffer::byte_buffer buf ){
	_read_buffer.reserve( _read_buffer.length() + buf.length());
	_read_buffer.write( buf.rd_ptr() , buf.length());

	while( _read_buffer.length() > sizeof(header)){	
		header hdr;
		_read_buffer.peak( &hdr , sizeof(hdr ));
		if ( _read_buffer.length() < sizeof(hdr) + hdr.len  ){
			break;
		} 
		_read_buffer >> hdr;
		tcode::buffer::byte_buffer ucbuf( hdr.orig_len );
		uncompress( ucbuf.wr_ptr() , &hdr.orig_len , _read_buffer.rd_ptr() , hdr.len );
		ucbuf.wr_ptr( hdr.orig_len );
		_read_buffer.rd_ptr( hdr.len );
		fire_filter_on_read( ucbuf );
	}	
	_read_buffer.fit();
}

void filter::filter_do_write( tcode::buffer::byte_buffer buf ){
	tcode::buffer::byte_buffer cbuf( static_cast<int>(buf.length() * 1.2 ) + sizeof(header));
	header hdr;
	hdr.len = 0;
	hdr.orig_len = buf.length();
	uLongf com_len = 0;
	compress( cbuf.rd_ptr() + sizeof(header) , &hdr.len , buf.rd_ptr() , buf.length() );
	cbuf << hdr;
	cbuf.wr_ptr( hdr.len );
	fire_filter_do_write( cbuf );
}

}}