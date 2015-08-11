#ifndef __tcode_time_tick_h__
#define __tcode_time_tick_h__
#include <stdint.h>
namespace tcode{

class time_span;
//! 1970년 1 월 1 일 0 시 이후 microsecond 단위로 1식 증가 
//! utc 기준 
//! local time / dst 등은 외부 클래스로 처리
class time_stamp{
public:
	time_stamp( void );
	time_stamp( const time_stamp& rhs );
	explicit time_stamp( const int64_t tick );

	time_stamp& operator=( const time_stamp& rhs );

	~time_stamp( void );

	int64_t tick( void ) const;
#if defined(_WIN32) || defined(_WIN64)
	 //1601/01/01 ~ 1970/01/01
	static const int64_t TICK_OFFSET = -11644473600 * 1000 * 1000;
#else
	static const int64_t TICK_OFFSET = 0;
#endif
public:
	bool operator==( const time_stamp& rhs ) const;
	bool operator!=( const time_stamp& rhs ) const;
	bool operator> ( const time_stamp& rhs ) const;
	bool operator>=( const time_stamp& rhs ) const;
	bool operator< ( const time_stamp& rhs ) const;
	bool operator<=( const time_stamp& rhs ) const;

	time_stamp operator+( const time_span& rhs ) const;
	time_stamp operator-( const time_span& rhs ) const;
	time_stamp& operator+=( const time_span& rhs ) ;
	time_stamp& operator-=( const time_span& rhs ) ;

	time_span  operator-( const time_stamp& rhs ) const;
public:
	static time_stamp now( void );

private:
	int64_t _tick;
};

}

#include <common/time_span.hpp>

#endif
