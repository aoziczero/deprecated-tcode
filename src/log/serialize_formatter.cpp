#include "stdafx.h"
#include <tcode/log/serialize_formatter.hpp>
#include <tcode/log/record.hpp>

namespace tcode { namespace log {

serialize_formatter::serialize_formatter( void ){
}

serialize_formatter::~serialize_formatter( void ){
}

void serialize_formatter::format( const record& r , tcode::byte_buffer& buf ){
	buf.reserve(8192);
	buf << r;
}

}}
