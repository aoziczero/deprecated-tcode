#ifndef __tcode_time_time_span_h__
#define __tcode_time_time_span_h__

namespace tcode {

class time_stamp;
class time_span {
public:
	time_span( void );
	explicit time_span( const int64_t delta );
	time_span( const time_span& rhs );
	time_span& operator=( const time_span& rhs );
	
	~time_span( void );

public:
	bool operator==( const time_span& rhs ) const;
	bool operator!=( const time_span& rhs ) const;
	bool operator> ( const time_span& rhs ) const;
	bool operator>=( const time_span& rhs ) const;
	bool operator< ( const time_span& rhs ) const;
	bool operator<=( const time_span& rhs ) const;
public:
	time_span operator+( const time_span& rhs ) const;
	time_span operator-( const time_span& rhs ) const;
	time_span operator*( const int times) const;
	time_span operator/( const int times) const;

	time_span& operator+=( const time_span& rhs ) ;
	time_span& operator-=( const time_span& rhs ) ;
	time_span& operator*=( const int times) ;
	time_span& operator/=( const int times) ;
	
	int64_t operator/( const time_span& rhs ) const;
	time_span operator-( void ) const;
	time_stamp operator+( const time_stamp& rhs ) const;
public:
	int64_t total_days( void ) const;
	int64_t total_hours( void ) const;
	int64_t total_minutes( void ) const;
	int64_t total_seconds( void ) const;
	int64_t total_milliseconds( void ) const;
	int64_t total_microseconds( void ) const;
public:
	static time_span days( int64_t c );
	static time_span hours( int64_t c );
	static time_span minutes( int64_t c );
	static time_span seconds( int64_t c );
	static time_span milliseconds( int64_t c );
	static time_span microseconds( int64_t c );

private:
	int64_t _delta;
};

}

#endif