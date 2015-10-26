#ifndef __tcode_io_ip_tcp_openssl_filter_h__
#define __tcode_io_ip_tcp_openssl_filter_h__

#include <tcode/io/ip/tcp/pipeline/filter.hpp>
#if defined ( TCODE_ENABLE_OPENSSL )
#include <openssl/ssl.h>

namespace tcode { namespace io { namespace ip { namespace tcp {

class openssl_filter : public filter{
public:
		enum class HANDSHAKE{
		ACCEPT , 
		CONNECT ,
		COMPLETE ,
	};

	openssl_filter( SSL_CTX* ctx , HANDSHAKE hs );
	virtual ~openssl_filter( void );

	virtual void filter_on_open( const tcode::io::ip::address& addr );
	virtual void filter_on_read( tcode::byte_buffer buf );
	virtual void filter_on_write( int written , bool flush );
	virtual void filter_do_write( tcode::byte_buffer buf );	
	
	bool is_fatal_error( int ret );
	void send_pending( void );
	bool write_rbio( tcode::byte_buffer msg );
	bool write_wbio( tcode::byte_buffer msg );
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

}}}}

#endif
#endif
