#include "stdafx.h"
#include "io.hpp"

namespace tcode { namespace io {


iovec make_buffer( void* ptr , int size ){
	iovec vec;

#if defined(TCODE_TARGET_WINDOWS)
	vec.buf = static_cast< char* >( ptr );
	vec.len = size;
#else
	vec.iov_base = static_cast< char* >( ptr );
	vec.iov_len = size;
#endif
	return vec;
}


iovec read_buffer( tcode::buffer::byte_buffer& buf ){
	iovec vec;

#if defined(TCODE_TARGET_WINDOWS)
	vec.buf = static_cast< char* >( static_cast< void* >(buf.wr_ptr()));
	vec.len = buf.space();
#else
	vec.iov_base = static_cast< char* >( static_cast< void* >(buf.wr_ptr()));
	vec.iov_len = buf.space();
#endif
	return vec;
}

iovec write_buffer( tcode::buffer::byte_buffer& buf ){
	iovec vec;

#if defined(TCODE_TARGET_WINDOWS)
	vec.buf = static_cast< char* >( static_cast< void* >(buf.rd_ptr()));
	vec.len = buf.length();
#else
	vec.iov_base = static_cast< char* >( static_cast< void* >(buf.rd_ptr()));
	vec.iov_len = buf.length();
#endif
	return vec;
}



}}