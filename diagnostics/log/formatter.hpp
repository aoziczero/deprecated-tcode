#ifndef __tcode_diagnostics_log_formatter_h__
#define __tcode_diagnostics_log_formatter_h__

#include <common/rc_ptr.hpp>
#include <buffer/byte_buffer.hpp>

namespace tcode { namespace diagnostics { namespace log {

struct record;
class formatter
	: public tcode::ref_counted< formatter> 
{
public:
	formatter( void );
	virtual ~formatter( void );
	virtual void format( const record& r , tcode::buffer::byte_buffer& buf );
private:
	
};

typedef tcode::rc_ptr< formatter > formatter_ptr;

formatter_ptr default_formatter( void );

}}}

#endif