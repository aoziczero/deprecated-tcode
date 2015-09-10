#ifndef __tcode_diagnostics_log_console_writer_h__
#define __tcode_diagnostics_log_console_writer_h__

#include <tcode/log/writer.hpp>
#include <tcode/byte_buffer.hpp>

namespace tcode { namespace log {

class console_writer : public writer
{
public:
	console_writer( void );
	virtual ~console_writer( void );
	virtual void write( const record& r );
private:	
	tcode::byte_buffer _buffer;
public:
	static std::shared_ptr< writer > instance( void );
};

}}

#endif
