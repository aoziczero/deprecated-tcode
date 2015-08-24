#include "stdafx.h"
#include "context.hpp"

#include <openssl/err.h>
#include <openssl/x509v3.h>
#include <threading/spin_lock.hpp>
#include <vector>

namespace tcode {
namespace ssl {

void  ssl_info_callback(const SSL *s, int where, int ret);
int pem_password(char *buf, int size, int rwflag, void *userdata) ;

//int password_cb(char *buf, int size, int rwflag, void *password)
//{
//    memcpy(buf, (char *)(password), size);
//    buf[size - 1] = '\0';
//    return strlen(buf);
//}

context::context(const SSL_METHOD* method)
	: _impl( SSL_CTX_new( method ))
{
	SSL_CTX_set_verify(_impl, SSL_VERIFY_NONE, nullptr);
	SSL_CTX_set_default_passwd_cb( _impl , &pem_password );
	SSL_CTX_set_default_passwd_cb_userdata( _impl , this );
	SSL_CTX_set_info_callback(_impl,ssl_info_callback);	
}

context::~context(void)
{
	SSL_CTX_free( _impl );
}

bool context::use_generate_key(){	
	EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    EVP_PKEY_keygen_init(pctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, 2048);
    EVP_PKEY_keygen(pctx, &pkey);
    
	X509 *x509 = X509_new();
    X509_set_version(x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 0);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), (long)60*60*24*365);
    X509_set_pubkey(x509, pkey);

    X509_NAME *name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (const unsigned char*)"US", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char*)"KO", -1, -1, 0);
    X509_set_issuer_name(x509, name);
    X509_sign(x509, pkey, EVP_md5());
        
    SSL_CTX_use_certificate(_impl, x509);
    //SSL_CTX_set_default_passwd_cb(_impl, password_cb);
    SSL_CTX_use_PrivateKey(_impl, pkey);
        
    RSA *rsa=RSA_generate_key(512, RSA_F4, NULL, NULL); 
    SSL_CTX_set_tmp_rsa(_impl, rsa); 
    RSA_free(rsa);
	return true;
}

bool context::use_certificate_file( const std::string& file ) {
	if ( SSL_CTX_use_certificate_file(_impl
		, file.c_str()
		, SSL_FILETYPE_PEM) <= 0)
	{
		return false;
    }
	return true;
}

bool context::use_private_key_file( const std::string& file 
								   , const std::string& pw ) {
	_private_key_password = pw;
	if ( SSL_CTX_use_PrivateKey_file(_impl
		, file.c_str()
		, SSL_FILETYPE_PEM) <= 0)
	{
		return false;
    }
	return true;
}

bool context::check_private_key( void ) {
	if (!SSL_CTX_check_private_key(_impl)) {
		return false;
	}
	return true;
}

SSL_CTX* context::impl( void ) {
	return _impl;
}

const std::string& context::password( void ) {
	return _private_key_password;
}

int pem_password(char *buf, int size, int rwflag, void *userdata) {
	context* ctx = static_cast< context* >( userdata );
	memcpy( buf , ctx->password().c_str() , std::min(size,static_cast<int>(ctx->password().length())));
	buf[size - 1] = '\0';
	return(strlen(buf));
}

void  ssl_info_callback(const SSL *s, int where, int ret)
{
	/*
  char * writeString;
  int w;
  w = where & ~SSL_ST_MASK;

  if (w & SSL_ST_CONNECT)
    writeString="SSL_connect";
  else if (w & SSL_ST_ACCEPT)
    writeString="SSL_accept";
  else
    writeString="undefined";

    fprintf(stderr, "======== writeString = [%s]\n", writeString);

  if (where & SSL_CB_LOOP)
  {
    BIO_printf(errBIO,"%s:%s\n",writeString,SSL_state_string_long(s));
    fprintf(stderr, "======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", 
        writeString, SSL_state_string_long(s));
  }
  else if (where & SSL_CB_ALERT)
  { 
	writeString=(where & SSL_CB_READ)?"read":"write";
    BIO_printf(errBIO,"SSL3 alert %s:%s:%s\n",writeString,SSL_alert_type_string_long(ret),SSL_alert_desc_string_long(ret));
    fprintf(stderr, "======== writeString = [%s], SSL_alert_type_string_long(ret) = [%s], SSL_alert_desc_string_long(ret) = [%s]\n", 
          writeString, SSL_alert_type_string_long(ret), SSL_alert_desc_string_long(ret));
  }
  else if (where & SSL_CB_EXIT)
  { 
	if (ret == 0) {
      BIO_printf(errBIO,"%s:failed in %s\n",writeString,SSL_state_string_long(s));
      fprintf(stderr,"======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", 
          writeString, SSL_state_string_long(s));
    }
    else if (ret < 0)
    {
      BIO_printf(errBIO,"%s:error in %s\n",writeString,SSL_state_string_long(s));
      fprintf(stderr,"======== writeString = [%s], SSL_state_string_long(s) = [%s]\n", 
          writeString, SSL_state_string_long(s));
    }
  }
  return ;*/
}


}}
