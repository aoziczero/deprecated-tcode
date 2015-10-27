#ifndef __tcode_time_timespan_h__
#define __tcode_time_timespan_h__

namespace tcode {

class timestamp;
/*!
*	@class timespan
*	@brief 시간 간격
*	@author	tk
*/
class timespan {
public:
	timespan( void );
	explicit timespan( const int64_t delta );
	timespan( const timespan& rhs );
	timespan& operator=( const timespan& rhs );
	
	~timespan( void );

public:
	bool operator==( const timespan& rhs ) const;
	bool operator!=( const timespan& rhs ) const;
	bool operator> ( const timespan& rhs ) const;
	bool operator>=( const timespan& rhs ) const;
	bool operator< ( const timespan& rhs ) const;
	bool operator<=( const timespan& rhs ) const;
public:
	timespan operator+( const timespan& rhs ) const;
	timespan operator-( const timespan& rhs ) const;
	timespan operator*( const int times) const;
	timespan operator/( const int times) const;

	timespan& operator+=( const timespan& rhs ) ;
	timespan& operator-=( const timespan& rhs ) ;
	timespan& operator*=( const int times) ;
	timespan& operator/=( const int times) ;
	
	int64_t operator/( const timespan& rhs ) const;
	timespan operator-( void ) const;
	timestamp operator+( const timestamp& rhs ) const;
public:
	int64_t total_days( void ) const;
	int64_t total_hours( void ) const;
	int64_t total_minutes( void ) const;
	int64_t total_seconds( void ) const;
	int64_t total_milliseconds( void ) const;
	int64_t total_microseconds( void ) const;
public:
	static timespan days( int64_t c );
	static timespan hours( int64_t c );
	static timespan minutes( int64_t c );
	static timespan seconds( int64_t c );
	static timespan milliseconds( int64_t c );
	static timespan microseconds( int64_t c );

private:
	int64_t _delta;
};

}

#endif
