#include "stdafx.h"
#include <tcode/time/ctime_adapter.hpp>
#include <time.h>

namespace tcode {

    ctime_adapter::ctime_adapter( const timestamp& ts )
        : _timestamp(ts)
    {
    }


    time_t ctime_adapter::tick(void) const{
        return static_cast< time_t >(_timestamp.tick() / 1000 / 1000);
    }

    std::string ctime_adapter::ctime( void  )
    {
        time_t t = tick();
        char buf[64] = {0,};
        ctime_r( &t , buf );
        return std::string(buf);
    }
    std::string ctime_adapter::strftime( const std::string& fmt )
    {
        time_t t = tick();     
        char buf[128] = {0,};
        struct tm local;
        localtime_r( &t , &local );
        ::strftime( buf , 128 , fmt.c_str() ,&local );
        return std::string(buf);
    } 

}
