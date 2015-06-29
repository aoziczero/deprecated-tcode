//! 
//! \file	windows_category_impl.hpp
//! \brief	윈도우 에러 카테고리 구현	
//! 
#ifndef __windows_category_impl_h__
#define __windows_category_impl_h__

namespace tcode{
namespace diagnostics{

//! 
//! \class	windows_category_impl
//! \brief	윈도우 에러 카테고리 구현
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 06
//! 
class windows_category_impl : public error_category {
public:
	//! construct
	windows_category_impl();

	//! destruct
	virtual ~windows_category_impl() _NOEXCEPT;

	//! category 식별이름
	//! \return "windows_category"
	virtual const char *name() const _NOEXCEPT;

	//!
	//! \brief FormatMessage 함수를 이용하여\n
	//! 윈도우 에러코드를 문자열로 변환한다.
	//! \return 윈도우 에러 문자열	
	//! \param _Errval [in] windows error code ( (WSA)GetLastError())
	//!
	virtual std::string message( int _Errval ) const;

};


//! windows_category ref
std::error_category& windows_category();

}}


#endif