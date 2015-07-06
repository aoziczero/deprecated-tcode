#ifndef __tcode_diagnostics_log_record_h__
#define __tcode_diagnostics_log_record_h__

#include <common/time_stamp.hpp>
#include <string>

namespace tcode { namespace diagnostics { namespace log {

enum level {
	LOG_TRACE = 0x01,
	LOG_DEBUG = LOG_TRACE << 1,
	LOG_INFO  = LOG_DEBUG << 1,
	LOG_WARN  = LOG_INFO << 1,
	LOG_ERROR = LOG_WARN << 1 ,
	LOG_FATAL = LOG_ERROR << 1 ,
	LOG_ALL = LOG_TRACE | LOG_DEBUG | LOG_INFO | LOG_WARN | LOG_ERROR | LOG_FATAL ,
	LOG_OFF = 0,
};

#if defined( TCODE_TARGET_WINDOWS )
typedef int thread_id;
#else
typedef pthread_t thread_id;
#endif

struct record{
	tcode::time_stamp time_stamp;
	log::level level;
	int line;;
	thread_id   tid;
	char tag[64];
	char file[256];
	char function[256];
	char message[2048];	
};

}}}

#endif