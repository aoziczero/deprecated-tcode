#ifndef __tcode_diagnostics_log_serialize_formatter_h__
#define __tcode_diagnostics_log_serialize_formatter_h__

#include <diagnostics/log/formatter.hpp>

namespace tcode { namespace diagnostics { namespace log {

struct record;
class serialize_formatter
	: public formatter
{
public:
	serialize_formatter( void );
	virtual ~serialize_formatter( void );
	virtual void format( const record& r , tcode::buffer::byte_buffer& buf );
	void serialize( const record&r , tcode::buffer::byte_buffer& buf );
	void deserialize( tcode::buffer::byte_buffer& buf , record& r );
	
};

}}}

#endif