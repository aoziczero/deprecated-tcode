#include "stdafx.h"
#include "formatter.hpp"

#include <common/time_util.hpp>

#include "record.hpp"
#include <stdio.h>

namespace tcode { namespace diagnostics { namespace log {

const char* to_string( tcode::diagnostics::log::level lv );

formatter::formatter( void ) {

}

formatter::~formatter( void ) {

}

void formatter::format( const record& r , tcode::buffer::byte_buffer& buf ){
	tcode::time::systemtime st;
	tcode::time::convert_to( r.time_stamp , st );
	buf.reserve( 8192 );
#if defined( TCODE_TARGET_WINDOWS )
	int len = _snprintf_s(reinterpret_cast<char*>( buf.wr_ptr()) , buf.space() , _TRUNCATE
#else
	int len = snprintf(reinterpret_cast<char*>( buf.wr_ptr()) , buf.space()	
#endif
        , "[%04d%02d%02d %02d%02d%02d][%s][%s][%s][%s][%s:%d][%d]\n"
		, st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond
		, to_string(r.level)
		, r.tag
		, r.message
		, r.function
		, r.file
		, r.line
        , (int)r.tid
		);
	buf.wr_ptr( len );
}

formatter_ptr default_formatter( void ){
	static formatter_ptr ptr( new formatter());
	return ptr;
}

}}}