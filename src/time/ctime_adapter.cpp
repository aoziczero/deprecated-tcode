#include "stdafx.h"
#include <tcode/time/ctime_adapter.hpp>
#include <time.h>
#include <ctime>
namespace tcode {

    ctime_adapter::ctime_adapter( const timestamp& ts )
        : _timestamp(ts)
    {
    }


    time_t ctime_adapter::tick(void) const{
        return static_cast< time_t >(_timestamp.tick() / 1000 / 1000);
    }

    std::string ctime_adapter::ctime( void  ){
        time_t t = tick();
        char buf[64] = {0,};
#if defined( TCODE_WIN32 )
		ctime_s( buf , 64 , &t );
#else
        ctime_r( &t , buf );
#endif
		return std::string(buf);
    }
    
    std::string ctime_adapter::strftime( const std::string& fmt , bool local ) {
        time_t t = tick();     
        char buf[128] = {0,};
        struct tm tm;
#if defined( TCODE_WIN32 )
		if ( local )
            localtime_s(  &tm , &t );
        else
            gmtime_s( &tm , &t );
#else
        if ( local )
            localtime_r( &t , &tm );
        else
            gmtime_r( &t , &tm);
#endif
        ::strftime( buf , 128 , fmt.c_str() ,&tm);
        return std::string(buf);
    } 

}
