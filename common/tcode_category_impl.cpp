#include "stdafx.h"
#include "tcode_category_impl.hpp"

#include <sstream>

namespace tcode{
namespace diagnostics{

tcode_category_impl::tcode_category_impl(){
}

tcode_category_impl::~tcode_category_impl() _NOEXCEPT {
}

const char * tcode_category_impl
	::name() const _NOEXCEPT{
	return "tcode_category";
}

std::string tcode_category_impl::message(
	int condition ) const
{
	static const std::string _map[] =
	{
		std::string("success") ,
		std::string("tls_not_initialized") ,
		std::string("cancel") ,
		std::string("closed") ,
		std::string("epoll_err") ,
		std::string("epoll_hup") ,
		std::string("condition_fail") ,
		std::string("build_fail") ,
	};

	if ( condition >= static_cast<int>(error_code::begin) && 
	     condition <  static_cast<int>(error_code::end))
	{
		return _map[condition];
	}
	return "unknown error";	
}

bool tcode_category_impl::equivalent(
	const std::error_code& errcode 
	, int condition) const _NOEXCEPT 
{
	if (*this == errcode.category() && 
		errcode.value() == condition)
		return true;

	// 둘다 0 이면 에러 없음임..
	if ( condition == 0 && errcode.value() == 0 )
		return true;
	return false;
}


template < typename type_t >
struct category_impl {
	static tcode_category_impl tcode_category;
};

template < typename type_t >
tcode_category_impl category_impl<type_t>::tcode_category;

std::error_category& tcode_category() {
	return category_impl<int>::tcode_category;
}


}}

namespace std{

error_condition make_error_condition( tcode::diagnostics::error_code condition ) {
	return error_condition( static_cast<int>(condition)
		, tcode::diagnostics::tcode_category());
}
/*
error_code make_error_code( tcode::diagnostics::code code ) {
	return error_code( static_cast<int>(code)
		, tcode::diagnostics::tcode_category());
}
*/

}
