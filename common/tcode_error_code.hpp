//!
//! \file tcode_error.hpp
//!
#ifndef __tcode_error_code_h__
#define __tcode_error_code_h__

namespace tcode{
namespace diagnostics{

///! 라이브러리 에러 정의
enum class error_code{
	begin = 0 , //!< start marker
	success = 0, //!< 성공	
	tls_not_initialized,		
	cancel ,
	closed ,
	epoll_err ,
	epoll_hup ,
	condition_fail , 
	build_fail ,
	end //!< end marker
};


extern std::error_code success;
extern std::error_code tls_not_initialized;
extern std::error_code cancel;
extern std::error_code closed;
extern std::error_code epoll_err;
extern std::error_code epoll_hup;
extern std::error_code condition_fail;
extern std::error_code build_fail;

std::error_code platform_error( void );

}}



#endif
