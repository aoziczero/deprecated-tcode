#include "stdafx.h"
#include "openssl_error_category.hpp"
#include <openssl/err.h>
#include <sstream>

namespace tcode{
namespace ssl{


openssl_error_category_impl::openssl_error_category_impl(void) {

}

openssl_error_category_impl::~openssl_error_category_impl(void) _NOEXCEPT {

}

const char *openssl_error_category_impl::name() const _NOEXCEPT  {
	return "openssl_error_category_impl";
}

std::string openssl_error_category_impl::message(int _Errval) const  {
	char err_msg[4096];
	ERR_error_string( _Errval , err_msg );
	return std::string( err_msg );
}


std::error_category& openssl_error_category() {
	static openssl_error_category_impl impl;
	return impl;
}


std::error_code openssl_error( int ec ){
	return std::error_code( ec , openssl_error_category());
}

}}