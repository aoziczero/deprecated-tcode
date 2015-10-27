#ifndef __tcode_byte_buffer_h__
#define __tcode_byte_buffer_h__

#include <tcode/block.hpp>

namespace tcode {

/*!
	@class	byte_buffer
	@brief	memory block ���� ���� �������̽�
	@author tk
	@detail	��� �������̽� ( rd_ptr / wr_ptr / etc ) ���� ACE message_block ��� ����\n
	���� ����ϱ� ���ϵ��� ������ class\n
	���� ���۴� block �Ҵ��� ������ �̿�\n
	���� refrenceCounting ���� �����Ѵ�.
*/
class byte_buffer {
public:
	//! buffer �� ���� ���� ��ġ ���� tell / seek ���� ����
	struct position {
	public:
		position( void );
		position( const position& rhs );
		position operator=( const position& rhs );
		void set( std::size_t r , std::size_t w);
		void swap( position& pos );
	public:
		std::size_t read;
		std::size_t write;
	};

	byte_buffer( void );
	//! sz ��ŭ�� ���� �Ҵ�
	explicit byte_buffer( const std::size_t sz );
	//! len ��ŭ�� ���� �Ҵ��� buf �� ������ internal buffer�� ���� ó�� 
	byte_buffer( uint8_t* buf , const std::size_t len );

	byte_buffer( const byte_buffer& rhs );
	byte_buffer( byte_buffer&& rhs );
	
	byte_buffer& operator=( const byte_buffer& rhs );
	byte_buffer& operator=( byte_buffer&& rhs );

	~byte_buffer( void );

	void swap( byte_buffer& rhs );
	
	// buffer size
	std::size_t capacity( void ) const;
	
	// writable pointer
	uint8_t* wr_ptr( void );
	int wr_ptr( const int move );

	// readable pointer
	uint8_t* rd_ptr( void );
	int rd_ptr( const int move );

	//! fit data
	int fit( void );

	//! fit data & fit buffer
	int shrink_to_fit( void );

	//! data size
	std::size_t length( void );
	
	//! writable size
	std::size_t space( void );

	//! data clear
	void clear( void );

	//! capacity reserve
	void reserve( const int sz ) ;

	std::size_t read(void* dst , const std::size_t sz );
	std::size_t peak(void* dst , const std::size_t sz );
	std::size_t write( void* src , const std::size_t sz );

	position tell( void ) const;
	void     seek( const position& p );

	std::size_t write_msg( const char* msg );
	std::size_t write_msg( const wchar_t* msg );
	std::size_t write_fmt( const char* msg , ... );

	tcode::byte_buffer sub_buffer( const std::size_t start , const std::size_t len );
	//! shallow copy
	tcode::byte_buffer duplicate(void);
	//! deep copy
	tcode::byte_buffer copy(void);
private:
	byte_buffer( block::handle handle , const position& pos );
private:
	block::handle _block;
	position _pos;
};

/*!
	@brief POD serialization
*/
template < typename Object >
byte_buffer& operator<<( byte_buffer& buf , const Object& obj ) {
	buf.write( const_cast<Object*>( &obj ), sizeof( obj ));
	return buf;
}	

/*!
	@brief POD data deserialization
*/
template < typename Object >
byte_buffer& operator>>( byte_buffer& buf , Object& obj ) {
	buf.read( &obj , sizeof( obj ));
	return buf;
}	

}

#endif
