#include "stdafx.h"
#include <tcode/time/systemtime.hpp>
#include <tcode/time/util.hpp>

namespace tcode { namespace time {
   
    systemtime::systemtime( void ){
        memset( this , 0x00 , sizeof(*this));
    }

    systemtime::systemtime( const tcode::timestamp& ts ) {
        struct tm tm_date;
        convert_to( ts , tm_date );
        wHour     = tm_date.tm_hour;
        wMinute   = tm_date.tm_min ;
        wDay      = tm_date.tm_mday;
        wMonth    = tm_date.tm_mon + 1;
        wSecond   = tm_date.tm_sec ;
        wYear     = tm_date.tm_year + 1900;
        wMilliseconds = ( ts.tick() % (1000 * 1000)) / 1000 ;		
    }

    filetime::filetime( void ) 
        : dwHighDateTime(0) , dwLowDateTime(0)
    {
    }

    filetime::filetime( const tcode::timestamp& ts ) 
    {
        convert_to( ts , *this );
    }

}}
