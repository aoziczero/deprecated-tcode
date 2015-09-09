#include "stdafx.h"
#include "console_writer.hpp"
#include "record.hpp"

namespace tcode { namespace diagnostics { namespace log {

console_writer::console_writer( void )
{
	
}

console_writer::~console_writer( void ){

}

void console_writer::write( const record& r ){
	_buffer.clear();
	formatter()->format( r , _buffer );
#if defined( TCODE_TARGET_WINDOWS )
	DWORD writes = 0;
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE) , &info );
	enum color_code {
		BLACK, D_BLUE, D_GREEN, D_SKYBLUE, D_RED, D_VIOLET, D_YELLOW,
		GRAY, D_GRAY, BLUE, GREEN, SKYBLUE, RED, VIOLET, YELLOW, WHITE,
	};	
	switch( r.level ) {
	case level::LOG_TRACE: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , VIOLET  ); break;
	case level::LOG_DEBUG: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , SKYBLUE  ); break;
	case level::LOG_INFO : SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , GREEN  ); break;
	case level::LOG_WARN : SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , BLUE  ); break;
	case level::LOG_ERROR: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , RED ); break;
	case level::LOG_FATAL: SetConsoleTextAttribute( GetStdHandle(STD_OUTPUT_HANDLE) , VIOLET ); break;
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
	switch( r.level ) {
	case level::LOG_TRACE: std::cout << "\033[" << FG_YELLOW	<< "m"; break;
	case level::LOG_DEBUG: std::cout << "\033[" << FG_CYAN		<< "m"; break;
	case level::LOG_INFO : std::cout << "\033[" << FG_GREEN		<< "m"; break;
	case level::LOG_WARN : std::cout << "\033[" << FG_BLUE		<< "m"; break;
	case level::LOG_ERROR: std::cout << "\033[" << FG_RED		<< "m"; break;
	case level::LOG_FATAL: std::cout << "\033[" << FG_MAGENTA	<< "m"; break;
	}
	std::cout << (char*)_buffer.rd_ptr() << "\033[" << FG_DEFAULT  << "m" ;
#endif
	_buffer.clear();
}

tcode::rc_ptr< writer > console_writer::instance( void ){
	tcode::rc_ptr< writer > writer( new console_writer());
	return writer;
}

}}}
