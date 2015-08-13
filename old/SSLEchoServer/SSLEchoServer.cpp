// SSLEchoServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#if defined ( TCODE_TARGET_WINDOWS )
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "tcode.common.lib")
#pragma comment(lib, "tcode.threading.lib")
#pragma comment(lib, "tcode.buffer.lib")
#pragma comment(lib, "tcode.diagnostics.lib")
#pragma comment(lib, "tcode.io.lib")
#pragma comment(lib, "tcode.transport.lib")
#pragma comment(lib, "tcode.transport-ssl.lib")
#pragma comment(lib, "VC/libeay32MDd.lib")
#pragma comment(lib, "VC/ssleay32MTd.lib")
#endif

#include <transport/event_loop.hpp>
#include <transport/tcp/acceptor.hpp>
#include <transport/tcp/filter.hpp>
#include <transport/tcp/pipeline_builder.hpp>
#include <transport/tcp/pipeline.hpp>
#include <transport-ssl/openssl.hpp>
#include <transport-ssl/context.hpp>
#include <transport-ssl/ssl_filter.hpp>
#include <diagnostics/log/log.hpp>

class echo_filter 
	: public tcode::transport::tcp::filter
{
public:
	echo_filter( void ){
	}
	virtual ~echo_filter( void ){
	}
	virtual void filter_on_open( const tcode::io::ip::address& addr ){
		LOG_T("ECHO" , "filter_on_open %s" , addr.ip_address().c_str() );
	}
	virtual void filter_on_close( void ){
		LOG_T("ECHO" , "filter_on_close" );
	}
	virtual void filter_on_read( tcode::buffer::byte_buffer buf ){
		LOG_T("ECHO" , "filter_on_read" );
		fire_filter_do_write( buf );
	}
	virtual void filter_on_write( int written , bool flush ){
		LOG_T("ECHO" , "filter_on_write %d " , written );
	}
	virtual void filter_on_error( const std::error_code& ec ){
		LOG_D("ECHO" , "filter_on_error : %s" , ec.message().c_str());
	}
	virtual void filter_on_end_reference( void ){
		LOG_T("ECHO" , "filter_on_end_reference" );
		delete this;
	}
private:

};

class acceptor_handler_impl 
	: public tcode::transport::tcp::acceptor_handler
{
public:
	acceptor_handler_impl( tcode::transport::event_loop& l ) 
		: _loop( l )
		, _ssl_context( SSLv23_server_method() )
	{
		_ssl_context.use_generate_key();
	}
	
	virtual bool condition( tcode::io::ip::socket_type h , const tcode::io::ip::address& addr ){
		return true;
	}

	virtual void acceptor_on_error( const std::error_code& ec ){
		LOG_D("ECHO" , "acceptor_impl error : %s" , ec.message().c_str());
	}

	virtual bool build( tcode::transport::tcp::pipeline& p ) {
		p.add(new tcode::transport::tcp::ssl_filter( _ssl_context.impl()
			, tcode::transport::tcp::ssl_filter::HANDSHAKE::ACCEPT ));
		p.add( new echo_filter());
		return true;
	}

	virtual tcode::transport::event_loop& channel_loop( void ){	
		return _loop;
	}
private:
	tcode::transport::event_loop& _loop;
	tcode::ssl::context _ssl_context;
};

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);	

	tcode::ssl::openssl_init();
	tcode::diagnostics::log::logger::instance();
	tcode::transport::event_loop loop;
	tcode::transport::tcp::acceptor acceptor(loop);
	tcode::transport::tcp::acceptor_handler_ptr handler( new acceptor_handler_impl(loop));
	if ( acceptor.listen( tcode::io::ip::address::any( 7542  , AF_INET ) , handler )){
		loop.run();
	} else {
		LOG_E("ECHO" , "Listen Fail %s" , tcode::diagnostics::platform_error().message().c_str());
	}
	
	return 0;
}


