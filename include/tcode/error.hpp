#ifndef __tcode_error_h__
#define __tcode_error_h__

#include <system_error>

namespace tcode {

	extern std::error_code error_success;
    extern std::error_code error_aborted;
    extern std::error_code error_disconnected;
    extern std::error_code error_invalid;
    extern std::error_code error_pipeline_build_fail;
    extern std::error_code error_timeout;

    extern std::error_code error_openssl_handshake;
    extern std::error_code error_openssl_bio_read;
    extern std::error_code error_openssl_bio_write;
    extern std::error_code error_openssl_ssl_read;
    extern std::error_code error_openssl_ssl_write;

    std::error_code last_error( void );
    std::error_category& tcode_category( void );
#if defined( TCODE_WIN32 )
	std::error_category& windows_category();
#endif
}

#endif
