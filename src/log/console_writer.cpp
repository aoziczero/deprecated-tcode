#include "stdafx.h"
#include <tcode/log/console_writer.hpp>
#include <tcode/log/record.hpp>

namespace tcode {  namespace log {

console_writer::console_writer( void )
{
	
}

console_writer::~console_writer( void ){

}

void console_writer::write( const record& r ){
	_buffer.clear();
	formatter()->format( r , _buffer );
#if defined( TCODE_WIN32 )
	DWORD writes = 0;
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE) , &info );
	enum color_code {
		BLACK, D_BLUE, D_GREEN, D_SKYBLUE, D_RED, D_VIOLET, D_YELLOW,
		GRAY, D_GRAY, BLUE, GREEN, SKYBLUE, RED, VIOLET, YELLOW, WHITE,
	};	
	switch( r.type ) {
	case type::trace: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , VIOLET  ); break;
	case type::debug: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , SKYBLUE  ); break;
	case type::info : SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , GREEN  ); break;
	case type::warn : SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , BLUE  ); break;
	case type::error: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , RED ); break;
	case type::fatal: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , VIOLET ); break;
	}
	std::cout << (char*)_buffer.rd_ptr() ;
	SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , info.wAttributes );	
#else
	enum color_code {
		FG_DEFAULT = 39,  FG_BLACK = 30, FG_RED = 31, FG_GREEN = 32, FG_YELLOW = 33, 
		FG_BLUE = 34, FG_MAGENTA = 35, FG_CYAN = 36, FG_LIGHT_GRAY = 37,FG_DARK_GRAY = 90, 
		FG_LIGHT_RED = 91, FG_LIGHT_GREEN = 92, FG_LIGHT_YELLOW = 93, FG_LIGHT_BLUE = 94, 
		FG_LIGHT_MAGENTA = 95, FG_LIGHT_CYAN = 96, FG_WHITE = 97, BG_RED = 41, BG_GREEN = 42, 
		BG_BLUE = 44, BG_DEFAULT = 49
    };
	switch( r.type ) {
	case type::trace: std::cout << "\033[" << FG_YELLOW	<< "m"; break;
	case type::debug: std::cout << "\033[" << FG_CYAN		<< "m"; break;
	case type::info : std::cout << "\033[" << FG_GREEN		<< "m"; break;
	case type::warn : std::cout << "\033[" << FG_BLUE		<< "m"; break;
	case type::error: std::cout << "\033[" << FG_RED		<< "m"; break;
	case type::fatal: std::cout << "\033[" << FG_MAGENTA	<< "m"; break;
	}
	std::cout << (char*)_buffer.rd_ptr() << "\033[" << FG_DEFAULT  << "m" ;
#endif
	_buffer.clear();
}

std::shared_ptr< writer > console_writer::instance( void ){
    std::shared_ptr< writer > writer( new console_writer());
	return writer;
}

}}
