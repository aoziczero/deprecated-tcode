#include "stdafx.h"
#include "time_span.hpp"
#include "time_stamp.hpp"

namespace tcode{

time_span::time_span( void )
	: _delta(0)
{
}

time_span::time_span( const int64_t delta )
	: _delta(delta )
{
}

time_span::time_span( const time_span& rhs )
	: _delta( rhs._delta )
{
}

time_span& time_span::operator=( const time_span& rhs ){
	_delta = rhs._delta;
	return *this;
}

time_span::~time_span( void ){
}

bool time_span::operator==( const time_span& rhs ) const {
	return _delta == rhs._delta;
}
bool time_span::operator!=( const time_span& rhs ) const{
	return _delta != rhs._delta;
}
bool time_span::operator> ( const time_span& rhs ) const{
	return _delta > rhs._delta;
}
bool time_span::operator>=( const time_span& rhs ) const{
	return _delta >= rhs._delta;
}
bool time_span::operator< ( const time_span& rhs ) const{
	return _delta < rhs._delta;
}
bool time_span::operator<=( const time_span& rhs ) const{
	return _delta <= rhs._delta;
}


time_span time_span::operator+( const time_span& rhs ) const {
	return time_span( _delta + rhs._delta );
}
time_span time_span::operator-( const time_span& rhs ) const{
	return time_span( _delta - rhs._delta );
}
time_span time_span::operator*( const int times) const{
	return time_span( _delta * times );
}
time_span time_span::operator/( const int times) const{
	return time_span( _delta / times  );
}

time_span& time_span::operator+=( const time_span& rhs )  {
	_delta += rhs._delta;
	return *this;
}
time_span& time_span::operator-=( const time_span& rhs ) {
	_delta -= rhs._delta;
	return *this;
}
time_span& time_span::operator*=( const int times) {
	_delta *= times;
	return *this;
}
time_span& time_span::operator/=( const int times) {
	_delta /= times;
	return *this;
}
	
int64_t time_span::operator/( const time_span& rhs ) const {
	if( rhs._delta == 0 )
		return 0;
	return _delta / rhs._delta;
}
time_span time_span::operator-( void ) const {
	return time_span(-_delta);
}

time_stamp time_span::operator+( const time_stamp& rhs ) const {
	return rhs + *this;
}

int64_t time_span::total_days( void ) const {
	return _delta / (static_cast<int64_t>(24) * 60 * 60 * 1000 * 1000);
}
int64_t time_span::total_hours( void ) const {
	return _delta / (static_cast<int64_t>(60) * 60 * 1000 * 1000);
}
int64_t time_span::total_minutes( void ) const{
	return _delta / (60 * 1000 * 1000);
}
int64_t time_span::total_seconds( void ) const{
	return _delta / (1000 * 1000);
}
int64_t time_span::total_milliseconds( void ) const{
	return _delta / 1000;
}
int64_t time_span::total_microseconds( void ) const{
	return _delta;
}

time_span time_span::days( int64_t c ) {
	return time_span( c * 24 * 60 * 60 * 1000 * 1000 );
}

time_span time_span::hours( int64_t c ) {
	return time_span( c * 60 * 60 * 1000 * 1000 );
}

time_span time_span::minutes( int64_t c ) {
	return time_span( c * 60 * 1000 * 1000 );
}

time_span time_span::seconds( int64_t c ) {
	return time_span( c * 1000 * 1000 );
}

time_span time_span::milliseconds( int64_t c ) {
	return time_span( c * 1000 );
}

time_span time_span::microseconds( int64_t c ) {
	return time_span( c );
}

}