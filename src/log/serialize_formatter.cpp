#include "stdafx.h"
#include "serialize_formatter.hpp"
#include "record.hpp"

namespace tcode { namespace diagnostics { namespace log {

serialize_formatter::serialize_formatter( void )
{
	
}

serialize_formatter::~serialize_formatter( void ){

}

void serialize_formatter::format( const record& r , tcode::buffer::byte_buffer& buf ){
	buf.reserve(8192);
	buf << r;
}

}}}
