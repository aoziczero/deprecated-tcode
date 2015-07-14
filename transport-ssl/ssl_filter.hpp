#ifndef __tcode_ssl_filter_h__
#define __tcode_ssl_filter_h__


#include <transport/tcp/filter.hpp>
#include <openssl/ssl.h>

namespace tcode { namespace transport { namespace tcp {

class ssl_filter 
	: public tcode::transport::tcp::filter {
public:
	enum class HANDSHAKE{
		ACCEPT , 
		CONNECT ,
		COMPLETE ,
	};

	ssl_filter( SSL_CTX* ctx , HANDSHAKE hs );
	virtual ~ssl_filter( void );

	virtual void filter_on_open( const tcode::io::ip::address& addr );
	virtual void filter_on_read( tcode::buffer::byte_buffer buf );
	virtual void filter_do_write( tcode::buffer::byte_buffer buf );	

	bool is_fatal_error( int ret );
	void send_pending( void );
	bool write_rbio( tcode::buffer::byte_buffer msg );
	bool write_wbio( tcode::buffer::byte_buffer msg );
	void begin_handshake( void );
	void end_handshake( void );

	X509* peer_certificate( void );
	void  free_peer_certificate( X509* x );

	SSL* ssl( void );	
private:
	SSL* _ssl;
	BIO* _rbio;
	BIO* _wbio;
	HANDSHAKE _handshake;
	tcode::io::ip::address _addr;
};

}}}

#endif