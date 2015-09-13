#include "stdafx.h"
#include <tcode/log/logger.hpp>
#include <tcode/log/console_writer.hpp>
#include <tcode/time/systemtime.hpp>
#include <stdarg.h>

namespace tcode { namespace log {


    logger::logger( void )
        : _type( log::type::all )
    {
#if defined( _DEBUG ) || defined( DEBUG )
        add_writer( tcode::log::console_writer::instance());
#endif
    }

    logger::~logger ( void ){

    }

    log::type logger::type( void ) {
        return _type;
    }

    bool logger::enabled( log::type t ) {
        return ( _type & t ) == t ;
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

        log::record record( t , tag , file , function , line );
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
        log::record record( t , tag , file , function , line );
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
            tcode::time::systemtime st(r.ts);
            char buffer[4096];
            int len = _snprintf_s(buffer , 4096, _TRUNCATE
                    , "[%04d%02d%02d %02d%02d%02d][%c][%s][%s]\n" 
                    , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond
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
}}
