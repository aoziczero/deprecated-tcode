#ifndef __tcode_diagnostics_log_console_writer_h__
#define __tcode_diagnostics_log_console_writer_h__

#include <common/rc_ptr.hpp>

#include <diagnostics/log/writer.hpp>

namespace tcode { namespace diagnostics { namespace log {

class console_writer : public writer
{
public:
	console_writer( void );
	virtual ~console_writer( void );
	virtual void write( const record& r );
private:	
	tcode::buffer::byte_buffer _buffer;
public:
	static tcode::rc_ptr< writer > instance( void );
};

}}}

#endif