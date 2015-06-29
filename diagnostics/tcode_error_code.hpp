//!
//! \file tcode_error.hpp
//!
#ifndef __tcode_error_code_h__
#define __tcode_error_code_h__

namespace tcode{
namespace diagnostics{

#ifdef TCODE_CPP_0x11_SUPPORT
typedef std::error_code error_code;	
typedef std::error_category error_category;
#else
// threading ���� �����ϹǷ� ���� �ʿ��ϴٸ� common ���� �����ؾ���
#include <common/error_code.hpp>
#include <common/error_category.hpp>
#endif

///! ���̺귯�� ���� ����
enum class errc{
	begin = 0 , //!< start marker
	success = 0, //!< ����	
	tls_not_initialized,		
	cancel ,
	closed ,
	epoll_err ,
	epoll_hup ,
	condition_fail , 
	build_fail ,
	end //!< end marker
};


extern tcode::diagnostics::error_code success;
extern tcode::diagnostics::error_code tls_not_initialized;
extern tcode::diagnostics::error_code cancel;
extern tcode::diagnostics::error_code closed;
extern tcode::diagnostics::error_code epoll_err;
extern tcode::diagnostics::error_code epoll_hup;
extern tcode::diagnostics::error_code condition_fail;
extern tcode::diagnostics::error_code build_fail;

tcode::diagnostics::error_code platform_error( void );

}}



#endif
