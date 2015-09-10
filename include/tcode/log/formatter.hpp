#ifndef __tcode_diagnostics_log_formatter_h__
#define __tcode_diagnostics_log_formatter_h__

#include <tcode/byte_buffer.hpp>
#include <memory>

namespace tcode {  namespace log {

struct record;
class formatter
{
public:
	formatter( void );
	virtual ~formatter( void );
	virtual void format( const record& r , tcode::byte_buffer& buf );
private:
	
};

typedef std::shared_ptr< formatter > formatter_ptr;

formatter_ptr default_formatter( void );

}}

#endif
