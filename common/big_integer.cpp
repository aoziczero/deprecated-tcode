#include "stdafx.h"
#include "big_integer.hpp"
#include <string>

namespace tcode {

const int big_integer::kBits = 8;
const int big_integer::kMask = 0xff;
const int big_integer::kDDabbleBase = 50;
const int big_integer::KDDabbleAdd = 78;

big_integer::big_integer( void ) 
{

}

big_integer::big_integer( const big_integer& rhs )
	: _data( rhs._data )
{
	
}

big_integer::big_integer( big_integer&& r )
	: _data( std::move(r._data))
{
	
}

big_integer::big_integer( int value ) {
	for ( int i = 0 ; i < sizeof( value ) / sizeof( data_type ) ; ++i ) {
		_data.push_back( value & kMask );
		value = ( value >> kBits );
	}
}

big_integer::~big_integer( void ) 
{

}

void big_integer::shr( int shift ) {
	int srcoffset = shift / kBits;
	int count = static_cast<int>(_data.size());
	if ( srcoffset >= count ){
		for ( int i = 0 ; i < count ; ++i){
			_data[i] = 0;
		}
		return;
	}
	
	shift = shift % kBits;	
	int rshift = kBits - shift;

	data_type temp = _data[srcoffset];
	_data[0] = temp >> shift;
	for ( int i = 1 ; i + srcoffset < count ; ++i ) {
		temp = _data[i+srcoffset];
		_data[i-1] |= temp << rshift;
		_data[i] = temp >> shift;
	}
	for ( int i = count - srcoffset ; i < count ; ++i ) {
		_data[i] = 0;
	}
}

void big_integer::shl( int shift ) {
	int srcoffset = shift / kBits;
	int dst = (int)_data.size() - 1;
	
	if ( dst - srcoffset < 0){
		for ( std::size_t i = 0 ; i < _data.size() ; ++i){
			_data[i] = 0;
		}
		return;
	}

	shift = shift % kBits;
	int rshift = kBits - shift;

	data_type temp = _data[ dst - srcoffset ];
	_data[dst] = temp << shift;

	for ( int i = dst - 1 ; i >= srcoffset ; --i ) {
		temp = _data[i-srcoffset];
		_data[i+1] |= temp >> rshift;		
		_data[i] = temp << shift;
	}
	for ( int  i = 0 ; i < srcoffset ; ++i ) {
		_data[i] = 0;
	}
}

void big_integer::_or( data_type val ) {
	if ( _data.size() == 0 )
		return;
	_data[0] |= val;
}

void big_integer::_or( const big_integer& bi ) {
	// 같은 자리수로 확장
	int cnt = bi.size() -  size();
	if ( cnt > 0) {
		// 2의 보수 
		data_type tv = sign() < 0 ? kMask : 0;
		for ( int i = 0 ; i < cnt ; ++i ){
			_data.push_back(tv);
		}
	}

	for ( int i = 0 ; i < bi.size(); ++i ) {
		_data[i] |= bi._data[i];	
	}
}

void big_integer::_and( data_type val ) {
	if ( _data.size() == 0 )
		return;
	_data[0] &= val;
}

void big_integer::_and( const big_integer& bi ){
	// 같은 자리수로 확장
	int cnt = bi.size() -  size();
	if ( cnt > 0) {
		// 2의 보수 
		data_type tv = sign() < 0 ? kMask : 0;
		for ( int i = 0 ; i < cnt ; ++i ){
			_data.push_back(tv);
		}
	}
	for ( int i = 0 ; i < bi.size(); ++i ) {
		_data[i] &= bi._data[i];	
	}
}

void big_integer::_xor( data_type val ) {
	if ( _data.size() == 0 )
		return;
	_data[0] ^= val;
}

void big_integer::_xor( const big_integer& bi ){
	// 같은 자리수로 확장
	int cnt = bi.size() -  size();
	if ( cnt > 0) {
		// 2의 보수 
		data_type tv = sign() < 0 ? kMask : 0;
		for ( int i = 0 ; i < cnt ; ++i ){
			_data.push_back(tv);
		}
	}
	for ( int i = 0 ; i < bi.size(); ++i ) {
		_data[i] ^= bi._data[i];	
	}
}
	
void big_integer::_not( void ) {
	for ( int i = 0 ; i < size(); ++i ) {
		_data[i] = ~_data[i];
	}
}

void big_integer::twos_complement( void ){
	_not();
	add( 1 );
}

int big_integer::sign( void ) const {
	if ( valid_size() == 0 )
		return 0;
	if ( _data.back() >> ( kBits -1 )!= 0 )
		return -1;
	return 1;
}

void big_integer::sub( const big_integer& bi ){
	// 동일한 자리수를 위한 padding
	data_type tv = sign() < 0 ? kMask : 0;
	data_type bv = bi.sign() < 0 ? kMask : 0;

	if ( bi._data.size() > _data.size() ) {		
		int sz = bi._data.size() - _data.size();
		for ( int i = 0 ; i < sz ; ++i ){
			_data.push_back(tv);
		}
	}

	// 2의 보수 + 1
	bool carry = true;
	int bi_max_idx = bi.size();
	for ( int i = 0; i < size() ; ++i ) {
		data_type complement = i < bi_max_idx ? ~bi._data[i]: ~bv;
		_data[i] += complement;
		if (carry) {
			if ( _data[i] >= complement )
				carry = false;
			if ( _data[i] == kMask )
				carry = true;
			++_data[i];
		} else {
			if ( _data[i] < complement ) 
				carry = true;
		}
	}	
}


void big_integer::add( data_type val ) {
	add( val , 0 );
}

void big_integer::add( data_type val , int idx ){
	bool neg = sign() < 0;
	while ( idx >= size() ) {
		if ( neg )
			_data.push_back(kMask);
		else 
			_data.push_back( 0 );
	}
	for ( int i = idx ; i <  size() ; ++i ) {
		_data[i] += val;
		if ( _data[i] < val ){
			val = 1;
		} else {
			val = 0;
		}
		if ( val == 0 )
			break;
	}
	if ( !neg && val ) {
		_data.push_back(val);
	}
	if( !neg && sign() < 0 ) {
		_data.push_back(0);
	}
}

void big_integer::add( const big_integer& bi ){
	data_type tv = sign() < 0 ? kMask : 0;
	data_type bv = bi.sign() < 0 ? kMask : 0;

	bool neg = tv || bv;

	if ( bi.size() > size() ) {
		int sz = bi.size() - size();
		for ( int i = 0 ; i < sz ; ++i ){
			_data.push_back( tv );
		}
	}
	bool carry = false;
	int bi_max_idx = bi.size();
	 
	for ( int i = 0; i < size() ; ++i ) {
		data_type v = i < bi_max_idx ? bi._data[i]: bv;
		_data[i] += v;
		if ( carry ) {
			if ( _data[i] >= v )
				carry = false;
			if ( _data[i] == kMask )
				carry = true;
			++_data[i];
		} else {
			if ( _data[i] < v ) 
				carry = true;
		}
	}
	if ( carry && !neg ) {
		_data.push_back(1);		
	}
}

void big_integer::mul( data_type val ) {
	big_integer vv;
	for ( int i = 0 ; i < valid_size() ; ++i ){
		ddata_type value = _data[i] * val;
		vv.add(  value & kMask , i );
		vv.add(  value >> kBits , i + 1 );
	}
	swap(vv);
}

void big_integer::mul( big_integer& bi ) {
	bool this_neg = sign() < 0;
	if ( this_neg ){
		twos_complement();
	}
	bool bi_neg = bi.sign() < 0;
	if ( bi_neg ){
		bi.twos_complement();
	}
	big_integer vv;
	for ( int i = 0 ; i < valid_size() ; ++i ){
		for ( int j = 0 ; j < bi.valid_size() ; ++j ) {
			ddata_type value = _data[i] * bi._data[j];
			vv.add(  value & kMask , i + j );
			vv.add(  value >> kBits , i + j + 1 );
		}
	}
	if ( bi_neg )	
		bi.twos_complement();
	swap(vv);	
	if ( this_neg ^ bi_neg ) 
		twos_complement();
}


void big_integer::reserve( int count ) {
	_data.clear();
	for( int i = 0 ; i < count ; ++i ) {
		_data.push_back(0);
	}
}

void big_integer::shrink_to_fit( void ){
	// 2의 보수로 음수를 표현하므로 양수이고
	// 최상위 값의 표현범위가 음수범위 라면
	// 최상위에 0 값을 추가해서 양수로 표현
	int is_sign = sign();
	while (!_data.empty()) {
		if ( _data.back() == 0 ) {
			_data.pop_back();
		} else {
			break;
		}
	}
	if ( is_sign > 0 ) {
		if ( _data.back() > kMask / 2 ){
			_data.push_back(0);
		}
	}
}

void big_integer::swap( big_integer& bi ) {
	_data.swap(bi._data);
}

int big_integer::valid_size( void ) const {
	int idx = size() - 1;
	while( idx >= 0 ){
		if ( _data[idx] != 0 )
			return idx + 1;
		--idx;
	}
	return idx + 1;
}

int big_integer::size( void ) const {
	return _data.size();
}

std::string big_integer::to_string( void ) {
	bool is_neg = sign() < 0;
	if ( is_neg ) 
		twos_complement();

	big_integer print;
	print.reserve( size() * 2);
	int idx = size() - 1	;
	for ( ; idx >= 0 ; --idx ) {
		for ( int bit = kBits - 1 ; bit >= 0 ; --bit ) {
			for ( int j = 0 ; j < print.size() ; ++j ) {
				if ( print._data[j] >= kDDabbleBase )
					print._data[j] += KDDabbleAdd;
			}
			print.shl(1);
			if ( _data[idx] & 1 << bit )
				print._or(1);
		}
	}
	std::string val;
	bool start = false;
	for ( int j = print.size() - 1 ; j >= 0 ; --j ) {
		if ( print._data[j] != 0 )
			start = true;
		if ( start ){
			std::string vv = std::to_string( print._data[j]);
			if ( vv.length() != 2 && val.length() != 0) {
				vv = "0" + vv;
			}
			val += vv;
		}
	}
	if ( is_neg ){
		val = "-" + val;
		twos_complement();
	}
	return val;	
}


uint64_t big_integer::get_low_uint64_value( void ) const  {
	uint64_t value=0;	
	int idx = std::min(  sizeof( uint64_t ) / sizeof( data_type ) 
				, _data.size() -1 );
	
	for ( ; idx >= 0 ; --idx ){
		value <<= kBits;	
		value |= _data[idx];
	}
	return value;
}

uint32_t big_integer::get_low_uint32_value( void ) const{
	uint32_t value=0;	
	int idx = std::min(  sizeof( uint32_t ) / sizeof( data_type ) 
				, _data.size() -1 );
	for ( ; idx >= 0 ; --idx ){
		value <<= kBits;
		value |= _data[idx];
	}
	return value;
}

/*
big_integer big_integer::operator>> ( int shift ) {
	big_integer ret(*this);
	ret.shr( shift );
	return ret;
}

big_integer big_integer::operator<< ( int shift ){
	big_integer ret(*this);
	ret.shl(shift);
	return ret;
}

big_integer& big_integer::operator>>= ( int shift ) {
	this->shr( shift );
	return *this;
}

big_integer& big_integer::operator<<= ( int shift ) {
	this->shl( shift );
	return *this;
}
*/
}
/*
 500000000 
1647483648
----------
2147483648

        50
        78*/