#include "stdafx.h"
#include "record.hpp"


namespace tcode { namespace diagnostics { namespace log {

const char* to_string( tcode::diagnostics::log::level lv ) {
	switch( lv ) {
	case LOG_TRACE:
		return "T";
	case LOG_DEBUG:
		return "D";
	case LOG_INFO:
		return "I";
	case LOG_WARN:
		return "W";
	case LOG_ERROR:
		return "E";
	case LOG_FATAL:
		return "F";	
	}
	return "?";
}

tcode::buffer::byte_buffer& operator<<( tcode::buffer::byte_buffer& buf , const record& r ) {
	buf << r.time_stamp.tick();
	buf << r.level;
	buf << r.line;
	buf << r.tid;
	short len = strlen( r.tag );	buf << len;		buf.write( const_cast<char*>(r.tag)  , len );
	len = strlen( r.file );			buf << len;		buf.write( const_cast<char*>(r.file)  , len );
	len = strlen( r.function );		buf << len;		buf.write( const_cast<char*>(r.function) , len );
	len = strlen( r.message );		buf << len;		buf.write( const_cast<char*>(r.message)  , len );
	return buf;
}	

tcode::buffer::byte_buffer& operator>>( tcode::buffer::byte_buffer& buf , record& r ) {
	int64_t tick;
	buf >> tick;
	r.time_stamp = tcode::time_stamp( tick );
	buf >> r.level;
	buf >> r.line;
	buf >> r.tid;
	short len;
	buf >> len; 	buf.read( r.tag , len );		r.tag[len] = 0;
	buf >> len;		buf.read( r.file  , len );		r.file[len] = 0;
	buf >> len;		buf.read( r.function  , len );	r.function[len] = 0;
	buf >> len;		buf.read( r.message  , len );	r.message[len] = 0;
	return buf;
}	

}}}