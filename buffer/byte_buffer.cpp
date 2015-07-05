#include "stdafx.h"
#include "byte_buffer.hpp"
#include <stdarg.h>
namespace tcode {
namespace buffer {


byte_buffer::position::position( void )
	: read(0) , write(0)
{
}
byte_buffer::position::position( const byte_buffer::position& rhs )
	: read(rhs.read) , write(rhs.write)
{
}

byte_buffer::position byte_buffer::position::operator=( const byte_buffer::position& rhs ) {
	read = rhs.read;
	write = rhs.write;
	return *this;
}

void byte_buffer::position::set( std::size_t r , std::size_t w) {
	read = r;
	write = w;
}

void byte_buffer::position::swap( position& pos ) {
	std::swap( read , pos.read );
	std::swap( write , pos.write );
}


byte_buffer::byte_buffer( void )
	: _block(nullptr) 
{
}

byte_buffer::byte_buffer( const std::size_t sz )
	: _block( block::alloc(sz) ) 
{
}

byte_buffer::byte_buffer( const byte_buffer& rhs )
	: _block( block::duplicate(rhs._block))
	, _pos( rhs._pos ) 
{
	
}

byte_buffer::byte_buffer( byte_buffer&& rhs )
	: _block( rhs._block )
	, _pos( rhs._pos ) 
{
	rhs._block = nullptr;
	rhs._pos.set(0,0);
}
	
byte_buffer& byte_buffer::operator=( const byte_buffer& rhs ) {
	block::handle b = block::duplicate( rhs._block );
	block::free( _block );
	_block = b;
	_pos = rhs._pos;
	return *this;
}

byte_buffer& byte_buffer::operator=( byte_buffer&& rhs ) {
	block::handle b = block::duplicate( rhs._block );
	block::free( _block );
	_block = b;
	_pos = rhs._pos;
	
	block::free( rhs._block );
	rhs._block = nullptr;
	rhs._pos.set(0,0);
	return *this;
}

byte_buffer::~byte_buffer( void ) {
	block::free( _block );
}

void byte_buffer::swap( byte_buffer& rhs ) {
	std::swap( _block , rhs._block );
	_pos.swap(rhs._pos);
}

std::size_t byte_buffer::capacity(void) const {
	return block::capacity( _block );
}


// writable pointer
uint8_t* byte_buffer::wr_ptr(void) {
	assert( _block != nullptr && "wr_ptr buffer is nullptr" );
	return reinterpret_cast< uint8_t* >(_block) + _pos.write;
}

int byte_buffer::wr_ptr(const int move){
	int realmove = 0;
	if (move >= 0) {
		realmove = std::min(move, (int)space());
		_pos.write += realmove;
	} else {
		realmove = std::max(move, -(int)length());
		_pos.write += realmove;
	}
	return realmove;
}

// readable pointer
uint8_t* byte_buffer::rd_ptr(void){
	assert(_block != nullptr && "rd_ptr buffer is nullptr");
	return reinterpret_cast< uint8_t* >(_block) + _pos.read;
}

int byte_buffer::rd_ptr(const int move){
	int realmove = 0;
	if (move >= 0) {
		realmove = std::min(move, (int)length());
		_pos.read += realmove;
	}else {
		realmove = std::max(move, -(int)_pos.read);
		_pos.read += realmove;
	}
	return realmove;
}

int byte_buffer::fit( void ) {
	memmove(_block , rd_ptr(), length());
	_pos.write = length();
	_pos.read = 0;
	return 0;
}

int byte_buffer::shrink_to_fit(void){
	block::handle new_b = block::alloc( length());
	memcpy(new_b  , rd_ptr() , length());
	block::free( _block );
	new_b = _block;
	int ret = (int)_pos.read;
	_pos.write = length();
	_pos.read = 0;
	return ret;
}

// data size
std::size_t byte_buffer::length(void){
	return _pos.write - _pos.read;
}

// writable size
std::size_t byte_buffer::space(void){
	return capacity() - _pos.write;
}

void byte_buffer::clear( void ) {
	_pos.read = _pos.write = 0;
}

void byte_buffer::reserve( const int sz ) {
	if ( block::refcount( _block ) == 1 
		&& sz < static_cast<int>(capacity())) {
		return;
	}	
	byte_buffer nb( sz );
	if ( _block != nullptr ) {
		nb.write( rd_ptr() , length() );
	}
	swap( nb );
}

std::size_t byte_buffer::read(void* dst, const std::size_t sz) {
	std::size_t cpysize = peak( dst , sz );
	rd_ptr(cpysize);
	return cpysize;
}

std::size_t byte_buffer::peak(void* dst, const std::size_t sz){
	std::size_t cpysize = std::min(sz, length());
	memcpy( dst , rd_ptr() , cpysize );
	return cpysize;
}

std::size_t byte_buffer::write(void* src, const std::size_t sz){
	std::size_t cpysize = std::min(sz, space());
	memcpy( wr_ptr() , src , cpysize );
	return wr_ptr(cpysize);
}

byte_buffer::position byte_buffer::tell( void ) const {
	byte_buffer::position p(_pos);
	return p;
}

void     byte_buffer::seek( const byte_buffer::position& p ) {
	// todo boundery check
	_pos = p;
}


std::size_t byte_buffer::write_fmt( const char* msg , ... ) {
	char fmt_buffer[2048] = { 0 , };
	va_list args;
	va_start( args , msg );
#if defined(TCODE_TARGET_WINDOWS)
	int len = _vsnprintf_s( fmt_buffer , 2048 , msg , args );
#else
	int len = vsnprintf( fmt_buffer , 2048 , msg , args );
#endif
	va_end( args );
	return write( fmt_buffer , len );
}

std::size_t byte_buffer::write( const char* msg ) {
	return write( const_cast<char*>(msg) , strlen(msg));
}

std::size_t byte_buffer::write( const wchar_t* msg ) {
	return write( const_cast<wchar_t*>( msg ) , wcslen( msg ) * sizeof( wchar_t ));
}

}}
