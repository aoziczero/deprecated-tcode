#include "stdafx.h"
#include <tcode/log/formatter.hpp>
#include <tcode/log/record.hpp>
#include <tcode/time/systemtime.hpp>
#include <stdio.h>

namespace tcode { namespace log {

formatter::formatter( void ) {

}

formatter::~formatter( void ) {

}

void formatter::format( const record& r , tcode::byte_buffer& buf ){
    tcode::time::systemtime st(r.ts);
    buf.reserve( 8192 );
#if defined( TCODE_WIN32 )
	int len = _snprintf_s(reinterpret_cast<char*>( buf.wr_ptr()) , buf.space() , _TRUNCATE
#else
	int len = snprintf(reinterpret_cast<char*>( buf.wr_ptr()) , buf.space()	
#endif
        , "[%04d%02d%02d %02d%02d%02d][%c][%s][%s][%s][%s:%d][%d]\n"
		, st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond
		, acronym(r.type)
		, r.tag
		, r.message
		, r.function
		, r.file
		, r.line
        , r.tid
		);
	buf.wr_ptr( len );
}

formatter_ptr default_formatter( void ){
	static formatter_ptr ptr( new formatter());
	return ptr;
}

}}
