#include "stdafx.h"
#include <tcode/log/logger.hpp>
#include <tcode/log/console_writer.hpp>
#include <stdarg.h>

namespace tcode { namespace log {


logger::logger( void )
	: _type( log::type::all )
{
#if defined( _DEBUG ) || defined( DEBUG )
//	add_writer( tcode::log::console_writer::instance());
#endif
}

logger::~logger ( void ){

}

log::type logger::type( void ) {
	return _type;
}

bool logger::enabled( log::type t ) {
	return ( _type & lv ) == t ;
}

void logger::enable( log::type t ){
	_type = static_cast<log::type >(_type|t);
}
	
void logger::write( 
	log::type t 
	, const char* tag
	, const char* file 
	, const char* function 		
	, const int line 
	, const char* msg
	, ... )
{
	if (!enabled(t))
		return;

	log::record record;
	record.ts = tcode::timestamp::now();
	record.type = t;
#if defined( TCODE_WIN32 )
	strcpy_s(record.tag , tag );
	strcpy_s(record.file , file  );
	strcpy_s(record.function , function );
#else
	strncpy(record.tag , tag , 32 );
	strncpy(record.file , file  , 256);
	strncpy(record.function , function , 256);
#endif
	record.line = line;
	record.tid = 
#if defined( TCODE_WIN32 )
		GetCurrentThreadId();
#else
		pthread_self();
#endif
	va_list args;
	va_start( args , msg );
#if defined( TCODE_WIN32 )
	_vsnprintf_s( record.message , 2048 , msg , args );
#else
	vsnprintf( record.message , 2048 , msg , args );
#endif
	va_end( args );		
	write(record);
}


void logger::write_dump( 
	log::type t 
	, const char* tag
	, const char* file 
	, const char* function 		
	, const int line 
    , uint8_t* buffer 
    , int sz
	, const char* msg
	, ... )
{
	if (!enabled(t))
		return;
	log::record record;
	record.ts = tcode::timestamp::now();
	record.type = t;
#if defined( TCODE_WIN32 )
	strcpy_s(record.tag , tag );
	strcpy_s(record.file , file  );
	strcpy_s(record.function , function );
#else
	strncpy(record.tag , tag , 32 );
	strncpy(record.file , file  , 256);
	strncpy(record.function , function , 256);
#endif
	record.line = line;
	record.tid = 
#if defined( TCODE_WIN32 )
		GetCurrentThreadId();
#else
		pthread_self();
#endif
	struct i_to_hex {
        static void convert( uint8_t ch , char* out ) {
            const char map[] = "0123456789ABCDEF";
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
#if defined( TCODE_WIN32 )
	_vsnprintf_s( record.message + index , 2048 - index , _TRUNCATE , msg , args );
#else
	vsnprintf( record.message + index , 2048 - index , msg , args );
#endif
	va_end( args );		
	write(record);
}

void logger::write( const record& r ) {
#if defined( TCODE_WIN32 )
	if ( IsDebuggerPresent() ) {
        tcode::ctime_adapter c(r.ts);
        std::strint time = c.ctime();
		char buffer[4096];
		int len = _snprintf_s(buffer , 4096, _TRUNCATE
			, "[%s][%s][%s][%s]\n" 
            , time.c_str()
			, acronym(r.type)
			, r.tag
			, r.message
			);		
		OutputDebugStringA( buffer );
	}	    
#endif
    tcode::threading::spinlock::guard guard( _lock );
	for (  std::size_t i = 0 ; i < _writers.size() ; ++i ) {
		_writers[i]->write( r );
	}
}

void logger::add_writer( const writer_ptr& ptr ){
    tcode::threading::spinlock::guard guard( _lock );
	_writers.push_back( ptr );
}

logger& logger::instance( void ){
	static logger instance;
	return instance;
}

}}}
