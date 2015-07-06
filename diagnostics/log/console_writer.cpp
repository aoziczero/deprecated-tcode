#include "stdafx.h"
#include "console_writer.hpp"

namespace tcode { namespace diagnostics { namespace log {

console_writer::console_writer( void )
{
	
}

console_writer::~console_writer( void ){

}

void console_writer::write( const record& r ){
	_buffer.clear();
	formatter()->format( r , _buffer );
	std::cout << (char*)_buffer.rd_ptr() << std::endl;
}

}}}
