#ifndef __tcode_diagnostics_log_writer_h__
#define __tcode_diagnostics_log_writer_h__

#include <common/rc_ptr.hpp>

#include <diagnostics/log/formatter.hpp>

namespace tcode { namespace diagnostics { namespace log {

struct record;
class writer
	: public tcode::ref_counted< writer> 
{
public:
	writer( void );
	virtual ~writer( void );
	virtual void write( const record& r ) = 0;
	formatter_ptr formatter( void );
	void formatter( const formatter_ptr& f );
private:
	formatter_ptr _formatter;
};

typedef tcode::rc_ptr< writer > writer_ptr;

}}}

#endif