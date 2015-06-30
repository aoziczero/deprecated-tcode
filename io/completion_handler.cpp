#include "stdafx.h"
#include "completion_handler.hpp"

namespace tcode { namespace io {

completion_handler::completion_handler( void ) {
	prepare();
}

completion_handler::~completion_handler( void ) {

}
	
tcode::diagnostics::error_code completion_handler::error_code( void ){
#if defined(TCODE_TARGET_WINDOWS)
	return std::error_code( 
		static_cast<int>(Internal)
		, *(std::error_category*)InternalHigh);
#else
	return _error_code;
#endif 
}

void completion_handler::error_code( const tcode::diagnostics::error_code& ec ) {
#if defined(TCODE_TARGET_WINDOWS)
	Internal = ec.value();
	InternalHigh = reinterpret_cast< ULONG_PTR >( &ec.category() );
#else
	_error_code = ec;
#endif 
}

void completion_handler::prepare( void ){
#if defined(TCODE_TARGET_WINDOWS)
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
	hEvent = 0;
#endif	
}

TCODE_SLIST_NODE_IMPL( completion_handler )

}}