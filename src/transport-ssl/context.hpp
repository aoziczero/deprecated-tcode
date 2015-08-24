#ifndef __tcode_ssl_context_h__
#define __tcode_ssl_context_h__

#include <openssl/ssl.h>
#include <iostream>

namespace tcode{
namespace ssl {

class context {
public:
	context( const SSL_METHOD* method );
	~context(void);
	bool use_generate_key();
	bool use_certificate_file( const std::string& file );
	bool use_private_key_file( const std::string& file
		, const std::string& pw );
	bool check_private_key( void );

	SSL_CTX* impl( void );
	const std::string& password( void );
private:
	SSL_CTX* _impl;
	std::string _private_key_password;	
};


}}
#endif

