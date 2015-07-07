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
	serialize( r , buf );
}

void serialize_formatter::serialize( const record&r , tcode::buffer::byte_buffer& buf ){
	buf << r.time_stamp.tick();
	buf << r.level;
	buf << r.line;
	buf << r.tid;
	int len = strlen( r.tag );
	buf << len;
	buf.write( const_cast<char*>(r.tag)  , len );
	len = strlen( r.file );
	buf << len;
	buf.write( const_cast<char*>(r.file)  , len );
	len = strlen( r.function );
	buf << len;
	buf.write( const_cast<char*>(r.function)  , len );
	len = strlen( r.message );
	buf << len;
	buf.write( const_cast<char*>(r.message)  , len );
}

void serialize_formatter::deserialize( tcode::buffer::byte_buffer& buf , record& r ){
	int64_t tick;
	buf >> tick;
	r.time_stamp = tcode::time_stamp( tick );
	buf >> r.level;
	buf >> r.line;
	buf >> r.tid;
	int len;
	buf >> len; 	buf.read( r.tag , len );		r.tag[len] = 0;
	buf >> len;		buf.read( r.file  , len );		r.file[len] = 0;
	buf >> len;		buf.read( r.function  , len );	r.function[len] = 0;
	buf >> len;		buf.read( r.message  , len );	r.message[len] = 0;
}


}}}
