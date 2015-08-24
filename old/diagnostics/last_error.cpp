#include "stdafx.h"
#include "last_error.hpp"
#include <thread>

namespace tcode { namespace diagnostics {
namespace {
	
#if defined( TCODE_TARGET_WINDOWS )
__declspec(thread) int code;
__declspec(thread) const error_category* cate;
#else
thread_local tcode::diagnostics::error_code tls_ec;
#endif

}

tcode::diagnostics::error_code last_error( void ){
#if defined( TCODE_TARGET_WINDOWS )
	if ( cate == nullptr )
		return tcode::diagnostics::error_code();
	return tcode::diagnostics::error_code(code,*cate);;
#else
	return tls_ec;
#endif
}

void last_error( const tcode::diagnostics::error_code& ec ){
#if defined( TCODE_TARGET_WINDOWS )
	code = ec.value();
	cate = &ec.category();
#else
	tls_ec = ec;
#endif
}

}}