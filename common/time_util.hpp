#ifndef __tcode_time_util_h__
#define __tcode_time_util_h__

namespace tcode {
class time_stamp;
namespace time {
#if defined( TCODE_TARGET_WINDOWS )
	typedef SYSTEMTIME systemtime;
	typedef FILETIME filetime;
	struct timespec {
		long    tv_sec;         /* seconds */
		long    tv_nsec;        /* nanoseconds */
	};
#else
	struct systemtime {
		uint16_t wYear;
		uint16_t wMonth;
		uint16_t wDayOfWeek;
		uint16_t wDay;
		uint16_t wHour;
		uint16_t wMinute;
		uint16_t wSecond;
		uint16_t wMilliseconds;
	};
	struct filetime {
		uint32_t dwLowDateTime;
		uint32_t dwHighDateTime;
	};
#endif
	bool is_leap_year( int year );
	// [year_start , year_end]
	int  number_of_days_between( int year_start , int year_end );
	// [mon_start , mon_end]
	int  number_of_days_between( int year , int mon_start , int mon_end );
	int  number_of_days( int year , int month );
	void convert_to( const time_t t , struct tm& tm );

	void convert_to( const tcode::time_stamp& st , time_t& out);
	void convert_to( const tcode::time_stamp& st , tm& out);
	void convert_to( const tcode::time_stamp& st , timeval& out);
	void convert_to( const tcode::time_stamp& st , timespec& out);
	void convert_to( const tcode::time_stamp& st , systemtime& out);
	void convert_to( const tcode::time_stamp& st , filetime& out);
	
}}


#endif