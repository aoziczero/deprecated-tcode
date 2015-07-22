#ifndef __tcode_big_integer_h__
#define __tcode_big_integer_h__

#include <vector>

namespace tcode {


class big_integer {
public:
	typedef unsigned char data_type;
	typedef unsigned short ddata_type;
	static const int kBits;
	static const int kMask;
	static const int kDDabbleBase;
	static const int KDDabbleAdd;
public:
	big_integer( void );
	big_integer( const big_integer& rhs );
	big_integer( big_integer&& r );

	explicit big_integer( int value );
	
	~big_integer( void );
	
	// 모든 연산은 성능이슈로 수행후 결과값을 가지는 형태
	// big_integer a( xxx );
	// big_integer b( xxx );
	// big_integer result( a );
	// result.add(b);
	// use result 형태로 사용가능
	// 사칙연사 helper 함수도 위와 같은 형태로 구현해도 됨
	void shr( int shift );
	void shl( int shift );
	
	void _or( data_type val );
	void _or( const big_integer& bi );

	void _and( data_type val );
	void _and( const big_integer& bi );

	void _xor( data_type val );
	void _xor( const big_integer& bi );

	void _not( void );

	void twos_complement( void );
	int  sign( void ) const;

	void sub( const big_integer& bi );

	void add( data_type val );

	void add( data_type val , int idx );
	void add( const big_integer& bi );

	void mul( data_type val );
	void mul( big_integer& bi );

	void div( data_type val );
	void div( const big_integer& bi );

	void reserve( int count );
	
	void shrink_to_fit( void );

	void swap( big_integer& bi );

	int valid_size( void ) const;
	int size( void ) const;
	std::string to_string( void ) ;

	// 하위값 확인용도
	uint64_t get_low_uint64_value( void ) const;
	uint32_t get_low_uint32_value( void ) const;
public:
/*
	big_integer operator>> ( int shift );
	big_integer operator<< ( int shift );

	big_integer& operator>>= ( int shift );
	big_integer& operator<<= ( int shift );
	
	big_integer operator |( const data_type or );
	big_integer operator |( const big_integer& bi );

	big_integer& operator|=( const data_type or );
	big_integer& operator|=( const big_integer& bi );

	big_integer operator &( const data_type or );
	big_integer operator &( const big_integer& bi );

	big_integer& operator&=( const data_type or );
	big_integer& operator&=( const big_integer& bi );
*/	
private:
	std::vector< data_type > _data;
};

}
#endif