#ifndef __tcode_diagnostics_log_serialize_formatter_h__
#define __tcode_diagnostics_log_serialize_formatter_h__

#include <tcode/log/formatter.hpp>

namespace tcode { namespace log {

struct record;
class serialize_formatter
	: public formatter
{
public:
	serialize_formatter( void );
	virtual ~serialize_formatter( void );
	virtual void format( const record& r , tcode::byte_buffer& buf );
};

}}

#endif
