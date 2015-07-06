#ifndef __tcode_diagnostics_log_logger_h__
#define __tcode_diagnostics_log_logger_h__

#include <common/rc_ptr.hpp>
#include <threading/spin_lock.hpp>
#include <vector>

#include <diagnostics/log/writer.hpp>

namespace tcode { namespace diagnostics { namespace log {


class logger {
public:
	logger( void );
	~logger ( void );
private:
	tcode::threading::spin_lock _lock;
	std::vector< writer_ptr > _writers;
};

}}}

#endif