#include "stdafx.h"
#include <tcode/time/timespan.hpp>
#include <tcode/time/timestamp.hpp>

namespace tcode{

timespan::timespan( void )
	: _delta(0)
{
}

timespan::timespan( const int64_t delta )
	: _delta(delta )
{
}

timespan::timespan( const timespan& rhs )
	: _delta( rhs._delta )
{
}

timespan& timespan::operator=( const timespan& rhs ){
	_delta = rhs._delta;
	return *this;
}

timespan::~timespan( void ){
}

bool timespan::operator==( const timespan& rhs ) const {
	return _delta == rhs._delta;
}
bool timespan::operator!=( const timespan& rhs ) const{
	return _delta != rhs._delta;
}
bool timespan::operator> ( const timespan& rhs ) const{
	return _delta > rhs._delta;
}
bool timespan::operator>=( const timespan& rhs ) const{
	return _delta >= rhs._delta;
}
bool timespan::operator< ( const timespan& rhs ) const{
	return _delta < rhs._delta;
}
bool timespan::operator<=( const timespan& rhs ) const{
	return _delta <= rhs._delta;
}


timespan timespan::operator+( const timespan& rhs ) const {
	return timespan( _delta + rhs._delta );
}
timespan timespan::operator-( const timespan& rhs ) const{
	return timespan( _delta - rhs._delta );
}
timespan timespan::operator*( const int times) const{
	return timespan( _delta * times );
}
timespan timespan::operator/( const int times) const{
	return timespan( _delta / times  );
}

timespan& timespan::operator+=( const timespan& rhs )  {
	_delta += rhs._delta;
	return *this;
}
timespan& timespan::operator-=( const timespan& rhs ) {
	_delta -= rhs._delta;
	return *this;
}
timespan& timespan::operator*=( const int times) {
	_delta *= times;
	return *this;
}
timespan& timespan::operator/=( const int times) {
	_delta /= times;
	return *this;
}
	
int64_t timespan::operator/( const timespan& rhs ) const {
	if( rhs._delta == 0 )
		return 0;
	return _delta / rhs._delta;
}
timespan timespan::operator-( void ) const {
	return timespan(-_delta);
}

timestamp timespan::operator+( const timestamp& rhs ) const {
	return rhs + *this;
}

int64_t timespan::total_days( void ) const {
	return _delta / (static_cast<int64_t>(24) * 60 * 60 * 1000 * 1000);
}
int64_t timespan::total_hours( void ) const {
	return _delta / (static_cast<int64_t>(60) * 60 * 1000 * 1000);
}
int64_t timespan::total_minutes( void ) const{
	return _delta / (60 * 1000 * 1000);
}
int64_t timespan::total_seconds( void ) const{
	return _delta / (1000 * 1000);
}
int64_t timespan::total_milliseconds( void ) const{
	return _delta / 1000;
}
int64_t timespan::total_microseconds( void ) const{
	return _delta;
}

timespan timespan::days( int64_t c ) {
	return timespan( c * 24 * 60 * 60 * 1000 * 1000 );
}

timespan timespan::hours( int64_t c ) {
	return timespan( c * 60 * 60 * 1000 * 1000 );
}

timespan timespan::minutes( int64_t c ) {
	return timespan( c * 60 * 1000 * 1000 );
}

timespan timespan::seconds( int64_t c ) {
	return timespan( c * 1000 * 1000 );
}

timespan timespan::milliseconds( int64_t c ) {
	return timespan( c * 1000 );
}

timespan timespan::microseconds( int64_t c ) {
	return timespan( c );
}

}
