#include "stdafx.h"

#include <tcode/io/ip/tcp/pipeline/filters/openssl_filter.hpp>
#include <tcode/io/ip/tcp/pipeline/channel.hpp>
#include <tcode/error.hpp>
#if defined ( TCODE_ENABLE_OPENSSL )
#include <openssl/err.h>

namespace tcode { namespace io { namespace ip { namespace tcp {

openssl_filter::openssl_filter( SSL_CTX* ctx  , HANDSHAKE hs ) 
	: _ssl( SSL_new( ctx ))
	, _rbio( BIO_new( BIO_s_mem()))
	, _wbio( BIO_new( BIO_s_mem()))
	, _handshake( hs )
{	
	SSL_set_bio( _ssl , _rbio , _wbio );
}

openssl_filter::~openssl_filter( void ){
	SSL_shutdown( _ssl );
	SSL_free( _ssl );
}

void openssl_filter::begin_handshake( void ) {	
	int ret = SSL_read( _ssl , nullptr , 0 );
	if ( is_fatal_error( ret ) ){
		end_handshake();
		channel()->close( tcode::error_openssl_handshake );
			//std::error_code( SSL_get_error(_ssl, ret) , tcode::ssl::openssl_error_category()));
		return;
	}
	send_pending();
}

void openssl_filter::end_handshake( void ) {
	if ( _handshake != HANDSHAKE::COMPLETE )
		fire_filter_on_open( _addr );
	_handshake = HANDSHAKE::COMPLETE;
}

void openssl_filter::filter_on_open( const tcode::io::ip::address& addr ){
	_addr = addr;
	if ( _handshake == HANDSHAKE::ACCEPT )
		SSL_set_accept_state( _ssl );
	else
		SSL_set_connect_state( _ssl );
	begin_handshake();
}

void openssl_filter::send_pending( void ) {
	while ( BIO_ctrl_pending( _wbio )) {
		tcode::byte_buffer mb( 4096 );
		int ret = BIO_read( _wbio , mb.wr_ptr() , mb.space() );
		if ( ret <= 0 ) {
			if (is_fatal_error(ret)){				
				channel()->close( tcode::error_openssl_bio_read );
			    //		std::error_code( SSL_get_error(_ssl, ret) , tcode::ssl::openssl_error_category()));
				end_handshake();
			}
			break;
		}else{
			mb.wr_ptr( ret );
			fire_filter_do_write(mb);
		}
	}
}

bool openssl_filter::write_rbio( tcode::byte_buffer msg ) {
	while( msg.length() > 0 ) {
		int len = BIO_write( _rbio 
			, msg.rd_ptr()
			, msg.length());

		if ( len <= 0 ) {
			if ( !BIO_should_retry(_rbio)){
				channel()->close( tcode::error_openssl_bio_write );
					//std::error_code( SSL_get_error(_ssl, len) , tcode::ssl::openssl_error_category()));
				end_handshake();
				return false;
			}
		} else {
			msg.rd_ptr( len );
		}
	}
	return true;
}

bool openssl_filter::write_wbio( tcode::byte_buffer msg  ) {
	while( msg.length() > 0 ) {
		int len = SSL_write( _ssl 
			, msg.rd_ptr()
			, msg.length());
		if ( len <= 0 ) {
			if ( is_fatal_error( len ) ) {
				channel()->close( tcode::error_openssl_ssl_write );
				//	std::error_code( SSL_get_error(_ssl, len) , tcode::ssl::openssl_error_category()));
				return false;
			}
		} else {
			msg.rd_ptr( len );
		}
	}
	return true;
}

void openssl_filter::filter_on_read( tcode::byte_buffer buf ){
	if ( !write_rbio( buf ) ) {
		return;
	}
	while ( true ) {
		tcode::byte_buffer mb(4096);
		int len = SSL_read( _ssl 
			, mb.wr_ptr() 
			, mb.space() );

		std::error_code ec;
		if ( len <= 0 ) {
			if ( is_fatal_error(len)){
                ec = tcode::error_openssl_ssl_read;
				//ec = std::error_code( SSL_get_error(_ssl, len) , tcode::ssl::openssl_error_category());
			}
		}
		if (( _handshake != HANDSHAKE::COMPLETE ) && SSL_is_init_finished( _ssl ) ) {
			end_handshake();
		}
		if ( len <= 0 ) {
			if ( ec ) {
				end_handshake();
				channel()->close( ec );
				return;
			}
			break;
		} else {
			mb.wr_ptr( len );
			fire_filter_on_read(mb);
		}		
	}
	send_pending();
}

void openssl_filter::filter_do_write( tcode::byte_buffer buf  ){
	write_wbio(buf);
	send_pending();
}

void openssl_filter::filter_on_write( int written , bool flush ){
	if ( _handshake == HANDSHAKE::COMPLETE ){
		fire_filter_on_write( written , flush );
	}
}

bool openssl_filter::is_fatal_error( int ret ) {
	int er_code = SSL_get_error(_ssl, ret);
	switch(er_code){
        case SSL_ERROR_NONE:
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_CONNECT:
        case SSL_ERROR_WANT_ACCEPT:
            return false;
    }
    // log!
	ERR_print_errors_fp(stderr);
	return true;
}


X509* openssl_filter::peer_certificate( void ) {
	return SSL_get_peer_certificate(_ssl);
}

void  openssl_filter::free_peer_certificate( X509* x ) {
	X509_free(x);
}

SSL* openssl_filter::ssl( void ) {
	return _ssl;
}

}}}}

#endif