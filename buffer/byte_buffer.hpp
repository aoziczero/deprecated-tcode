#ifndef __tcode_buffer_byte_buffer_h__
#define __tcode_buffer_byte_buffer_h__

#include <buffer/block.hpp>

namespace tcode {
namespace buffer {

class byte_buffer {
public:
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
	explicit byte_buffer( const std::size_t sz );

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

	int fit( void );

	int shrink_to_fit( void );

	// data size
	std::size_t length( void );
	
	// writable size
	std::size_t space( void );

	void clear( void );

	void reserve( const int sz ) ;

	std::size_t read(void* dst , const std::size_t sz );
	std::size_t peak(void* dst , const std::size_t sz );
	std::size_t write( void* src , const std::size_t sz );


	position tell( void ) const;
	void     seek( const position& p );

	std::size_t write_msg( const char* msg );
	std::size_t write_msg( const wchar_t* msg );
	std::size_t write_fmt( const char* msg , ... );
private:
	block::handle _block;
	position _pos;
};

template < typename Object >
byte_buffer& operator<<( byte_buffer& buf , const Object& obj ) {
	buf.write( const_cast<Object*>( &obj ), sizeof( obj ));
	return buf;
}	

}}

#endif
