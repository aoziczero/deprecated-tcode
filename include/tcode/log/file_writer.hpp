#ifndef __tcode_diagnostics_log_file_writer_h__
#define __tcode_diagnostics_log_file_writer_h__

#include <tcode/log/writer.hpp>
#include <tcode/byte_buffer.hpp>

namespace tcode { namespace log {

class file_writer 
	: public writer {
public:
	file_writer( void );
	virtual ~file_writer( void );
	virtual void write( const record& r );
private:
	bool _create_log_file();
	void _delete_old_logs();
private:
	tcode::byte_buffer _buffer;	
#if defined( TCODE_WIN32 )
	HANDLE			_file;
#else
	FILE*           _file;
#endif
	uint32_t		_current_day;
	uint32_t		_line_count;
	uint32_t		_day_file_count;
};

}}

#endif
