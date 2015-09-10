#ifndef __tcode_time_systemtime_h__
#define __tcode_time_systemtime_h__

#include <tcode/time/timestamp.hpp>

namespace tcode { namespace time {

    struct systemtime
#if defined( TCODE_WIN32 )
        : public SYSTEMTIME
#endif
    {
    public:
        systemtime( void );
        systemtime( const tcode::timestamp& ts );
    public:
#if !defined( TCODE_WIN32 )
        uint16_t wYear;
        uint16_t wMonth;
        uint16_t wDayOfWeek;
        uint16_t wDay;
        uint16_t wHour;
        uint16_t wMinute;
        uint16_t wSecond;
        uint16_t wMilliseconds;
#endif
    };

    struct filetime 
#if defined( TCODE_WIN32 )
        : public FILETIME
#endif
    {
    public:
        filetime( void );
        filetime( const tcode::timestamp& ts );
#if !defined( TCODE_WIN32 )
        uint32_t dwHighDateTime;
        uint16_t dwLowDateTime;
#endif
    };

}}

#endif
