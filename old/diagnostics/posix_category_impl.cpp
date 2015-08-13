#include "stdafx.h"
#include "posix_category_impl.hpp"

namespace tcode{
namespace diagnostics{


posix_category_impl::posix_category_impl()
{
}


posix_category_impl::~posix_category_impl()
{
}

const char * posix_category_impl
	::name() const _NOEXCEPT{
	return "posix_category";
}

std::string posix_category_impl
	::message( int _Errval ) const {
	const size_t buffer_size = 4096;
	char buffer[buffer_size] = {0,};
#if defined(TCODE_TARGET_WINDOWS)
	strerror_s( buffer , buffer_size , _Errval);
	return std::string( buffer );
#elif defined(TCODE_TARGET_LINUX)
	return std::string( strerror_r( _Errval , buffer , buffer_size));
#elif defined(TCODE_TARGET_MAC_OSX)

#endif

}

template < typename type_t >
struct category_impl {
	static posix_category_impl posix_category;
};

template < typename type_t >
posix_category_impl category_impl<type_t>::posix_category;

std::error_category& posix_category() {
	return category_impl<int>::posix_category;
}


}}
