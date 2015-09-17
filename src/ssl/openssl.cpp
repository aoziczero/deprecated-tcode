#include "stdafx.h"
#include <tcode/ssl/openssl.hpp>


#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509v3.h>
#include <tcode/threading/spinlock.hpp>
#include <vector>


struct CRYPTO_dynlock_value {
    tcode::threading::spinlock lock;
};


namespace tcode {  namespace ssl {


tcode::threading::spinlock* locks = nullptr;


void ssl_lock_callback(  int m 
					   , int n 
					   , const char* file 
					   , int line ) {
	if ( m & CRYPTO_LOCK ) {
		locks[n].lock();
	} else {
		locks[n].unlock();
	}
}

CRYPTO_dynlock_value* ssl_lock_dyn_create_callback(
	const char *file
	, int line)
{
    CRYPTO_dynlock_value *l = new CRYPTO_dynlock_value();
    return l;
}
 
void ssl_lock_dyn_callback(int mode
						   , CRYPTO_dynlock_value* l
						   , const char *file
						   , int line)
{
    if(mode & CRYPTO_LOCK)
        l->lock.lock();
    else
        l->lock.unlock();
}
 
void ssl_lock_dyn_destroy_callback(CRYPTO_dynlock_value* l
								   , const char *file
								   , int line)
{
    delete l;
}

bool openssl_init( void ){
	locks = new tcode::threading::spinlock[CRYPTO_num_locks()];

#ifdef _DEBUG
    CRYPTO_malloc_debug_init();
    CRYPTO_dbg_set_options(V_CRYPTO_MDEBUG_ALL);
    CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);
#endif
	CRYPTO_set_locking_callback( &ssl_lock_callback );
	CRYPTO_set_dynlock_create_callback(&ssl_lock_dyn_create_callback);
	CRYPTO_set_dynlock_lock_callback(&ssl_lock_dyn_callback); 
	CRYPTO_set_dynlock_destroy_callback(&ssl_lock_dyn_destroy_callback);

	SSL_load_error_strings();
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    SSL_library_init();
	
	return true;
}

}}

