#include "stdafx.h"
#include "tcode_category_impl.hpp"
#include "windows_category_impl.hpp"
#include "posix_category_impl.hpp"

namespace tcode {
namespace diagnostics{

tcode::diagnostics::error_code success( 
	static_cast<int>(tcode::diagnostics::errc::success) 
	, tcode_category());

tcode::diagnostics::error_code tls_not_initialized( 
	static_cast<int>(tcode::diagnostics::errc::tls_not_initialized) 
	, tcode_category());

tcode::diagnostics::error_code cancel(
	 static_cast<int>(tcode::diagnostics::errc::cancel)
	, tcode_category());

tcode::diagnostics::error_code closed(
	 static_cast<int>(tcode::diagnostics::errc::closed)
	, tcode_category());

tcode::diagnostics::error_code epoll_err(
	 static_cast<int>(tcode::diagnostics::errc::epoll_err)
	, tcode_category());

tcode::diagnostics::error_code epoll_hup(
	 static_cast<int>(tcode::diagnostics::errc::epoll_hup)
	, tcode_category());

tcode::diagnostics::error_code condition_fail(
	 static_cast<int>(tcode::diagnostics::errc::condition_fail)
	, tcode_category());

tcode::diagnostics::error_code build_fail(
	 static_cast<int>(tcode::diagnostics::errc::build_fail)
	, tcode_category());

tcode::diagnostics::error_code platform_error( void ) {
#ifdef TCODE_TARGET_WINDOWS
	return tcode::diagnostics::error_code( GetLastError() , windows_category());
#else
	return tcode::diagnostics::error_code( errno , posix_category());
#endif
}


}}
