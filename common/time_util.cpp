#include "stdafx.h"
#include "time_util.hpp"
#include "time_stamp.hpp"

namespace tcode {
namespace time {
namespace {

static const int MONTH_TABLE [2][12] = {
	{ 31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } , // common year
	{ 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } , // leap year
};

}

bool is_leap_year( int year ) {
	if ( (year%4) == 0 ) {
		if ( (year%100) == 0 ) {
			if ( (year%400) == 0 ) {
				return true;
			}
			return false;
		}
		return true;
	}
	return false;
}

int  number_of_days_between( int year_start , int year_end ) {
	if ( year_end < year_start )
		return 0;
	int year = year_start;
	int tot = 0;
	while ( year <= year_end ) {
		tot += is_leap_year( year ) ? 366 : 365;
		++year;
	}
	return tot;
}

int  number_of_days_between( int year , int mon_start , int mon_end  ) {
	if ( mon_end < mon_start )
		return 0;
	int idx = is_leap_year(year) ? 1 : 0;
	int mon = mon_start;
	int tot = 0;
	while ( mon <= mon_end ) {
		tot += MONTH_TABLE[idx][mon-1];
		++mon;
	}
	return tot;
}

int number_of_days( int year , int month ) {
	return MONTH_TABLE[ is_leap_year(year) ? 1 : 0][month-1];
}

void convert_to( const time_t t , struct tm& tm ){
	tm.tm_sec  = ( t % 60 );
	tm.tm_min  = ( t % ( 60 * 60 ) ) / 60;
	tm.tm_hour = ( t % ( 60 * 60 * 24 )) / ( 60 * 60 );
	tm.tm_wday = ((t / ( 60 * 60 * 24 )) + 4 ) % 7; // 0 day thursday

	int32_t daycount = static_cast< int32_t >( t / ( 60 * 60 * 24 ));
	int32_t year     = 1970;

	for(;;) {
		int32_t dayPerYear = is_leap_year( year ) ? 366 : 365;
		if ( daycount < dayPerYear )
			break;
		++year;
		daycount -= dayPerYear;
	}

	tm.tm_year = static_cast< int > (year - 1900);
	tm.tm_yday = static_cast< int > (daycount);
	int32_t month = 0;
	int monthTableIndex =  is_leap_year( year ) ? 1 : 0;
	for(;;) {
		if ( daycount < MONTH_TABLE[ monthTableIndex ][ month ] ){
			break;
		}
		daycount -= MONTH_TABLE[ monthTableIndex ][ month ];
		++month;
	}
	tm.tm_mon  = static_cast< int > (month);
	tm.tm_mday = static_cast< int > (daycount + 1);
	tm.tm_isdst = 0;
}


void convert_to( const tcode::time_stamp& st , time_t& out){
	out = static_cast< time_t >( st.tick() / ( 1000 * 1000 ));
}

void convert_to( const tcode::time_stamp& st , tm& out){
	convert_to( static_cast< time_t >( st.tick() / ( 1000 * 1000 )) , out );
}

void convert_to( const tcode::time_stamp& st , timeval& out){
	out.tv_sec = static_cast< long >( st.tick() / ( 1000 * 1000 ));
	out.tv_usec = st.tick() % ( 1000 * 1000 );
}

void convert_to( const tcode::time_stamp& st , timespec& out){
	out.tv_nsec =( st.tick() %  ( 1000 * 1000 ) ) * 1000;
	out.tv_sec  = static_cast< long >( st.tick()  /  ( 1000 * 1000 ) );
}

void convert_to( const tcode::time_stamp& st , systemtime& out){
#if defined(TCODE_TARGET_WINDOWS)
		filetime ft;
		convert_to( st , ft );
		FileTimeToSystemTime( &ft , &out );
#else
		struct tm tm_date;
		convert_to( st , tm_date );
		out.wHour     = tm_date.tm_hour;
		out.wMinute   = tm_date.tm_min ;
		out.wDay      = tm_date.tm_mday;
		out.wMonth    = tm_date.tm_mon + 1;
		out.wSecond   = tm_date.tm_sec ;
		out.wYear     = tm_date.tm_year + 1900;
		out.wMilliseconds = ( st.tick() % (1000 * 1000)) / 1000 ;		
#endif
}

void convert_to( const tcode::time_stamp& st , filetime& out){
	uint64_t tv = st.tick();
	tv -= time_stamp::TICK_OFFSET;
	tv *= 10;
	out.dwHighDateTime = ( tv & 0xFFFFFFFF00000000 ) >> 32;
	out.dwLowDateTime  = ( tv & 0xFFFFFFFF		   );
}
	
}}