#include "stdafx.h"
#include "logger.hpp"
#include <common/time_util.hpp>

#include <stdarg.h>

namespace tcode { namespace diagnostics { namespace log {

const char* to_string( tcode::diagnostics::log::level lv );

logger::logger( void )
	: _level( log::level::LOG_ALL )
{

}

logger::~logger ( void ){

}

log::level logger::log_level( void ) {
	return _level;
}

bool logger::enabled( log::level lv ) {
	return ( _level & lv ) != 0;
}

void logger::enable( log::level lv ){
	_level = static_cast<log::level>(_level|lv);
}
	
void logger::write( 
	log::level lv 
	, const char* tag
	, const char* file 
	, const char* function 		
	, const int line 
	, const char* msg
	, ... )
{
	if (!enabled(lv))
		return;

	log::record record;
	record.time_stamp = tcode::time_stamp::now();
	record.level = lv;
#if defined( TCODE_TARGET_WINDOWS )
	strcpy_s(record.tag , tag );
	strcpy_s(record.file , file  );
	strcpy_s(record.function , function );
#else
	strncpy(record.tag , tag , 64);
	strncpy(record.file , file  , 256);
	strncpy(record.function , function , 256);
#endif
	record.line = line;
	record.tid = 
#if defined( TCODE_TARGET_WINDOWS )
		GetCurrentThreadId();
#else
		pthread_self();
#endif
	va_list args;
	va_start( args , msg );
#if defined( TCODE_TARGET_WINDOWS )
	_vsnprintf_s( record.message , 2048 , msg , args );
#else
	vsnprintf( record.message , 2048 , msg , args );
#endif
	va_end( args );		
	write(record);
}


void logger::write_dump( 
	log::level lv 
	, const char* tag
	, const char* file 
	, const char* function 		
	, const int line 
    , uint8_t* buffer 
    , int sz
	, const char* msg
	, ... )
{
	if (!enabled(lv))
		return;
	log::record record;
	record.time_stamp = tcode::time_stamp::now();
	record.level = lv;
#if defined( TCODE_TARGET_WINDOWS )
	strcpy_s(record.tag , tag );
	strcpy_s(record.file , file  );
	strcpy_s(record.function , function );
#else
	strncpy(record.tag , tag , 64);
	strncpy(record.file , file  , 256);
	strncpy(record.function , function , 256);
#endif
	record.line = line;
	record.tid = 
#if defined( TCODE_TARGET_WINDOWS )
		GetCurrentThreadId();
#else
		pthread_self();
#endif
	struct i_to_hex {
        static void convert( uint8_t ch , char* out ) {
            static std::string map("0123456789ABCDEF");
            out[0] = map[ch/16];
            out[1] = map[ch%16];
        }
    };
	int index = 0;
	record.message[index++] = '\n';
	for (int i = 0 ; i < sz ; ++i ){
		i_to_hex::convert( buffer[i] , record.message + index );
		index += 2;
		switch( (i % 16) + 1 ){
            case 16:
				record.message[index++] = '\n';
                break;
            case 8:
                record.message[index++] = ' ';
            case 4:
            case 12:
                record.message[index++] = ' ';
            default:
                record.message[index++] = ' ';
        }
		if( index > 2040 )
			break;
	}
	record.message[index++] = '\n';
	va_list args;
	va_start( args , msg );
#if defined( TCODE_TARGET_WINDOWS )
	_vsnprintf_s( record.message + index , 2048 - index , _TRUNCATE , msg , args );
#else
	vsnprintf( record.message + index , 2048 - index , msg , args );
#endif
	va_end( args );		
	write(record);
}

void logger::write( const record& r ) {
	tcode::threading::scoped_lock<> guard( _lock );
#if defined( TCODE_TARGET_WINDOWS )
	if ( IsDebuggerPresent() ) {
		tcode::time::systemtime st;
		tcode::time::convert_to( r.time_stamp , st );
		char buffer[4096];
		int len = _snprintf_s(buffer , 4096, _TRUNCATE
			, "[%04d%02d%02d %02d%02d%02d][%s][%s][%s][%s][%s:%d][%d]\r\n"
			, st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond
			, to_string(r.level)
			, r.tag
			, r.message
			, r.function
			, r.file
			, r.line
			, (int)r.tid
			);		
		OutputDebugStringA( buffer );
	}	    
#endif
	for (  std::size_t i = 0 ; i < _writers.size() ; ++i ) {
		_writers[i]->write( r );
	}
}

void logger::add_writer( const writer_ptr& ptr ){
	tcode::threading::scoped_lock<> guard( _lock );
	_writers.push_back( ptr );
}

logger& logger::instance( void ){
	static logger instance;
	return instance;
}

}}}