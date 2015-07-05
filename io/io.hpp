#ifndef __tcode_io_h__
#define __tcode_io_h__

#include <buffer/byte_buffer.hpp>
#if defined(TCODE_TARGET_LINUX)
#include <sys/socket.h>
#endif
namespace tcode { 

#if defined(TCODE_TARGET_WINDOWS)
	typedef WSABUF iovec;
#else
	typedef iovec iovec;
#endif

namespace io {

tcode::iovec make_buffer( void* ptr , int size );
tcode::iovec read_buffer( tcode::buffer::byte_buffer& buf );
tcode::iovec write_buffer( tcode::buffer::byte_buffer& buf );

}}

#endif