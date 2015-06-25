#include "stdafx.h"
#include "time_util.hpp"

namespace tcode {
namespace {

static const int MONTH_TABLE [2][12] = {
	{ 31 , 28 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } , // common year
	{ 31 , 29 , 31 , 30 , 31 , 30 , 31 , 31 , 30 , 31 , 30 , 31 } , // leap year
};

}

bool time_util::is_leap_year( int year ) {
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

int  time_util::number_of_days_between( int year_start , int year_end ) {
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

int  time_util::number_of_days_between( int year , int mon_start , int mon_end  ) {
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

int time_util::number_of_days( int year , int month ) {
	return MONTH_TABLE[ is_leap_year(year) ? 1 : 0][month-1];
}

/*
time_t ctime::convert( const tm& aTm ) {
	time_t t = 0;
	t += aTm.tm_sec;
	t += aTm.tm_min  * 60;
	t += aTm.tm_hour * 60 * 60;
	t += aTm.tm_yday * 60 * 60 * 24;
	//[1970 , aT.tm_year + 1900) 이내의 날짜수 이므로 -1 해야한다.
	t += daycount( 1970 , ( aTm.tm_year + 1900 - 1 ) ) * (60 * 60 * 24);	
	//t -= ( 9 * 60 * 60 );
	return t;
}

struct tm ctime::convert( const time_t& aTime ) {
	struct tm aTm;
	aTm.tm_sec  = ( aTime % 60 );
	aTm.tm_min  = ( aTime % ( 60 * 60 ) ) / 60;
	aTm.tm_hour = ( aTime % ( 60 * 60 * 24 )) / ( 60 * 60 );
	aTm.tm_wday = ((aTime / ( 60 * 60 * 24 )) + 4 ) % 7; // 0 day thursday

	int32_t daycount = static_cast< int32_t >( aTime / ( 60 * 60 * 24 ));
	int32_t year     = 1970;

	for(;;) {
		int32_t dayPerYear = ctime::daycount( year );
		if ( daycount < dayPerYear )
			break;
		++year;
		daycount -= dayPerYear;
	}

	aTm.tm_year = static_cast< int > (year - 1900);
	aTm.tm_yday = static_cast< int > (daycount);
	int32_t month = 0;
	int monthTableIndex =  ctime::isleapyear( year ) ? 1 : 0;
	for(;;) {
		if ( daycount < DAY_PER_MONTH_TABLE[ monthTableIndex ][ month ] ){
			break;
		}
		daycount -= DAY_PER_MONTH_TABLE[ monthTableIndex ][ month ];
		++month;
	}
	aTm.tm_mon  = static_cast< int > (month);
	aTm.tm_mday = static_cast< int > (daycount + 1);
	aTm.tm_isdst = 0;
	return aTm;
}
*/
}