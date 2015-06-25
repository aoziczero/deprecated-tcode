#include "stdafx.h"
#include "time_stamp.hpp"

namespace tcode{

time_stamp::time_stamp( void ) 
	: _tick(0)
{

}

time_stamp::time_stamp( const time_stamp& rhs ) 
	: _tick( rhs._tick )
{
	
}

time_stamp::time_stamp( const int64_t tick ) 
	: _tick( tick )
{

}

time_stamp& time_stamp::operator=( const time_stamp& rhs ) {
	_tick = rhs._tick;
	return *this;
}

time_stamp::~time_stamp( void ) {
	
}

int64_t time_stamp::tick( void ) const {
	return _tick;
}

bool time_stamp::operator==( const time_stamp& rhs ) const {
	return _tick == rhs._tick;
}
bool time_stamp::operator!=( const time_stamp& rhs ) const {
	return _tick != rhs._tick;
}
bool time_stamp::operator> ( const time_stamp& rhs ) const{
	return _tick > rhs._tick;
}
bool time_stamp::operator>=( const time_stamp& rhs ) const{
	return _tick >= rhs._tick;
}
bool time_stamp::operator< ( const time_stamp& rhs ) const{
	return _tick < rhs._tick;
}
bool time_stamp::operator<=( const time_stamp& rhs ) const{
	return _tick <= rhs._tick;
}

time_stamp time_stamp::operator+( const time_span& rhs ) const {
	return time_stamp( _tick + rhs.total_microseconds() );
}
time_stamp time_stamp::operator-( const time_span& rhs ) const {
	return time_stamp( _tick - rhs.total_microseconds() );
}
time_stamp& time_stamp::operator+=( const time_span& rhs ) {
	_tick += rhs.total_microseconds();
	return *this;
}
time_stamp& time_stamp::operator-=( const time_span& rhs )  {
	_tick -= rhs.total_microseconds();
	return *this;
}

time_span  time_stamp::operator-( const time_stamp& rhs ) const {
	return time_span( _tick - rhs._tick );
}


time_stamp time_stamp::now( void ) {
#if defined(TCODE_TARGET_WINDOWS)
	FILETIME ft;
    GetSystemTimeAsFileTime( &ft );
	LARGE_INTEGER qp;
	qp.HighPart = ft.dwHighDateTime;
	qp.LowPart = ft.dwLowDateTime;
	int64_t value = qp.QuadPart / 10;
	//const int64_t MICROSEC_19700101_MINUS_16010101 = 11644473600 * 1000 * 1000;
	//value -= MICROSEC_19700101_MINUS_16010101;
	value += TICK_OFFSET;
	return time_stamp( value );
#else
	struct timespec ts;
	int64_t value= 0;
    if( clock_gettime( CLOCK_REALTIME , &ts ) == -1 ) {
        value = std::time(nullptr) * 1000 * 1000;
    } else {
		value = ( ts.tv_sec * 1000 * 1000 ) + (ts.tv_nsec / 1000);
	}
	return time_stamp(value);    
#endif
}

}
