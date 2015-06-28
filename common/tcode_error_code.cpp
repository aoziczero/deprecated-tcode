#include "stdafx.h"
#include "tcode_category_impl.hpp"
#include "windows_category_impl.hpp"
#include "posix_category_impl.hpp"

namespace tcode {
namespace diagnostics{

std::error_code success( 
	static_cast<int>(tcode::diagnostics::error_code::success) 
	, tcode_category());

std::error_code tls_not_initialized( 
	static_cast<int>(tcode::diagnostics::error_code::tls_not_initialized) 
	, tcode_category());

std::error_code cancel(
	 static_cast<int>(tcode::diagnostics::error_code::cancel)
	, tcode_category());

std::error_code closed(
	 static_cast<int>(tcode::diagnostics::error_code::closed)
	, tcode_category());

std::error_code epoll_err(
	 static_cast<int>(tcode::diagnostics::error_code::epoll_err)
	, tcode_category());

std::error_code epoll_hup(
	 static_cast<int>(tcode::diagnostics::error_code::epoll_hup)
	, tcode_category());

std::error_code condition_fail(
	 static_cast<int>(tcode::diagnostics::error_code::condition_fail)
	, tcode_category());

std::error_code build_fail(
	 static_cast<int>(tcode::diagnostics::error_code::build_fail)
	, tcode_category());

std::error_code platform_error( void ) {
#ifdef TCODE_TARGET_WINDOWS
	return std::error_code( GetLastError() , windows_category());
#else
	return std::error_code( errno , posix_category());
#endif
}


}}
