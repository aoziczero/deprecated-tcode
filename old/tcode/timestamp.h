#ifndef __tcode_timestamp_h__
#define __tcode_timestamp_h__

#include <tcode.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	s64 tick;
} tcode_timestamp_t;

typedef struct {
	s64 delta;
} tcode_timespan_t;

void tcode_timestamp_init( tcode_timestamp_t* ts );
void tcode_timestamp_now_utc( tcode_timestamp_t* ts );

#ifdef __cplusplus
}
#endif
#endif