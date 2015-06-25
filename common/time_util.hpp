#ifndef __tcode_time_util_h__
#define __tcode_time_util_h__

namespace tcode {

class time_util {
public:
	static bool is_leap_year( int year );
	// [year_start , year_end]
	static int  number_of_days_between( int year_start , int year_end );
	// [mon_start , mon_end]
	static int  number_of_days_between( int year , int mon_start , int mon_end );
	static int  number_of_days( int year , int month );
};

}


#endif