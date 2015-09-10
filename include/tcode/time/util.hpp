#ifndef __tcode_time_util_h__
#define __tcode_time_util_h__

#include <tcode/time/timestamp.hpp>
#include <tcode/time/systemtime.hpp>

namespace tcode { namespace time {

    bool is_leap_year( int year );
    int  number_of_days_between( int year_start , int year_end );
    int  number_of_days_between( int year , int mon_start , int mon_end );
    int number_of_days( int year , int month );

    void convert_to( const tcode::timestamp& st , time_t& out);
    void convert_to( const tcode::timestamp& st , tm& out);
    void convert_to( const tcode::timestamp& st , timeval& out);
    void convert_to( const tcode::timestamp& st , timespec& out);
    void convert_to( const tcode::timestamp& st , filetime& out);
    void convert_to( const tcode::timestamp& st , systemtime& out);
}}

#endif
