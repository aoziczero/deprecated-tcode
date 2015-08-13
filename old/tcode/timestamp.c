#include "timestamp.h"
#include "stdafx.h"

void tcode_timestamp_init( tcode_timestamp_t* ts ){
	ts->tick = 0;
}

void tcode_timestamp_now_utc( tcode_timestamp_t* ts ){
#if defined(TCODE_TARGET_WINDOWS)
	FILETIME ft;
    GetSystemTimeAsFileTime( &ft );
	LARGE_INTEGER qp;
	qp.HighPart = ft.dwHighDateTime;
	qp.LowPart = ft.dwLowDateTime;
	int64_t value = qp.QuadPart / 10;	
	value += ( -11644473600 * 1000 * 1000);
#else
	struct timespec sts;
	int64_t value= 0;
    if( clock_gettime( CLOCK_REALTIME , &sts ) == -1 ) {
        value = time(nullptr) * 1000 * 1000;
    } else {
		value = ( sts.tv_sec * 1000 * 1000 ) + (sts.tv_nsec / 1000);
	}
#endif
	ts->tick = value;
}