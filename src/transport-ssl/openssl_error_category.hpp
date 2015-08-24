#ifndef __tcode_openssl_error_category_impl_h__
#define __tcode_openssl_error_category_impl_h__

namespace tcode{
namespace ssl{

class openssl_error_category_impl : public std::error_category {
public:
	openssl_error_category_impl();
	virtual ~openssl_error_category_impl() _NOEXCEPT;
	virtual const char *name() const _NOEXCEPT;
	virtual std::string message( int _Errval ) const;
};


std::error_category& openssl_error_category();
std::error_code openssl_error( int ec );

}}


#endif