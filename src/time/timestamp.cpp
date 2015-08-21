#include "stdafx.h"
#include <tcode/time/timestamp.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/time/ctime_adapter.hpp>

#if defined( TCODE_WIN32 )

#elif defined( TCODE_LINUX )
    
#elif defined( TCODE_APPLE )
#include <mach/clock.h>
#include <mach/mach.com>
#endif

namespace tcode {

    timestamp::timestamp(const int64_t tick)
        : _tick( tick )
    {

    }

    timestamp::timestamp( const timestamp& rhs )
        : _tick( rhs.tick() )
    {
    }

    timestamp& timestamp::operator=( const timestamp& rhs ) {
        _tick = rhs.tick();
        return *this;
    }
    
    bool timestamp::operator==( const timestamp& rhs ) const {
        return _tick == rhs._tick;
    }

    bool timestamp::operator!=( const timestamp& rhs ) const {
        return _tick != rhs._tick;
    }
    
    bool timestamp::operator> ( const timestamp& rhs ) const{
        return _tick > rhs._tick;
    }

    bool timestamp::operator>=( const timestamp& rhs ) const{
        return _tick >= rhs._tick;
    }

    bool timestamp::operator< ( const timestamp& rhs ) const{
        return _tick < rhs._tick;
    }

    bool timestamp::operator<=( const timestamp& rhs ) const{
        return _tick <= rhs._tick;
    }

    timestamp timestamp::operator+( const timespan& rhs ) const {
        return timestamp( _tick + rhs.total_microseconds() );
    }

    timestamp timestamp::operator-( const timespan& rhs ) const {
        return timestamp( _tick - rhs.total_microseconds() );
    }

    timestamp& timestamp::operator+=( const timespan& rhs ) {
        _tick += rhs.total_microseconds();
        return *this;
    }

    timestamp& timestamp::operator-=( const timespan& rhs )  {
        _tick -= rhs.total_microseconds();
        return *this;
    }

    timespan  timestamp::operator-( const timestamp& rhs ) const {
        return timespan( _tick - rhs._tick );
    }



    int64_t timestamp::tick( void ) const {
        return _tick;
    }

    std::string timestamp::to_string( void ) const {
        return ctime_adapter(*this).strftime( "%Y-%m-%d, %H:%M:%S");
    }

    timestamp timestamp::now( void ) {
#if defined( TCODE_WIN32 )
        FILETIME ft;
        GetSystemTimeAsFileTime( &ft );
        LARGE_INTEGER qp;
        qp.HighPart = ft.dwHighDateTime;
        qp.LowPart = ft.dwLowDateTime;
        int64_t value = qp.QuadPart / 10;
	    static const int64_t TICK_OFFSET = -11644473600 * 1000 * 1000;
        value += TICK_OFFSET;
        return timestamp(value);
#elif defined( TCODE_LINUX )
        struct timespec ts;
        int64_t value= 0;
        if( clock_gettime( CLOCK_REALTIME , &ts ) == -1 ) {
            value = std::time(nullptr) * 1000 * 1000;
        } else {
            value = ( ts.tv_sec * 1000 * 1000 ) + (ts.tv_nsec / 1000);
        }
        return timestamp(value);    
#elif defined( TCODE_APPLE )
        struct timespec ts;
        int64_t value = 0;
        clock_serv_t cclock;
        mach_timespec_t mts;
        host_get_clock_service(mach_host_self() , CALENDAR_CLOCK , &cclock );
        clock_get_time( cclock , &mts );
        mach_port_deallocate(mach_task_self() , cclock );
        ts.tv_sec = mts.tv_sec;
        tv.tv_nsec = mts.tv_nsec;
        value = ( ts.tv_sec * 1000 * 1000 ) + (ts.tv_nsec / 1000);
        return timestamp(value);
#endif
    }


}
