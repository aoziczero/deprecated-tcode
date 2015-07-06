#include "stdafx.h"
#include "record.hpp"


namespace tcode { namespace diagnostics { namespace log {

const char* to_string( tcode::diagnostics::log::level lv ) {
	switch( lv ) {
	case LOG_TRACE:
		return "T";
	case LOG_DEBUG:
		return "D";
	case LOG_INFO:
		return "I";
	case LOG_WARN:
		return "W";
	case LOG_ERROR:
		return "E";
	case LOG_FATAL:
		return "F";	
	}
	return "?";
}

}}}