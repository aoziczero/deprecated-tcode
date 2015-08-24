#include "stdafx.h"
#include "windows_category_impl.hpp"

#include <sstream>

namespace tcode{
namespace diagnostics{

windows_category_impl::windows_category_impl(){
}

windows_category_impl::~windows_category_impl() _NOEXCEPT{
}

const char * windows_category_impl
	::name() const _NOEXCEPT{
	return "windows_category";
}

std::string windows_category_impl
	::message( int _Errval ) const {

	const size_t buffer_size = 4096;
    DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM;
    LPCVOID lpSource = NULL;
	char buffer[buffer_size] = {0,};
	unsigned long result;
    result = ::FormatMessageA(
        dwFlags,
        lpSource,
        _Errval,
        0,
        buffer,
        buffer_size,
        NULL);

    if (result == 0) {
        std::ostringstream os;
        os << "unknown error code: " << _Errval << ".";
        return os.str();
    } 
	char* pos = strrchr( buffer , '\r' );
	if ( pos ) 
		*pos = '\0';
	return std::string( buffer );
}

template < typename type_t >
struct category_impl {
	static windows_category_impl windows_category;
};

template < typename type_t >
windows_category_impl category_impl<type_t>::windows_category;

std::error_category& windows_category() {
	return category_impl<int>::windows_category;
}

}}