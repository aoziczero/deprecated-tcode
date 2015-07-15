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
#include <transport/tcp/connector.hpp>
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
		tcode::buffer::byte_buffer hello(10);
		hello.write_msg("Hello!\0");
		fire_filter_do_write(hello);
	}
	virtual void filter_on_close( void ){
		LOG_T("ECHO" , "filter_on_close" );
	}
	virtual void filter_on_read( tcode::buffer::byte_buffer buf ){
		buf.write_msg("\0");
		LOG_T("ECHO" , "filter_on_read %s" , buf.rd_ptr() );
		
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

class connector_impl : public tcode::transport::tcp::pipeline_builder
	, public tcode::transport::tcp::connector::error_handler
{
public:
	connector_impl( tcode::transport::event_loop& l ) 
		: _connector(  tcode::transport::tcp::connector::create( l 
			, tcode::rc_ptr<tcode::transport::tcp::connector::error_handler>(this)) )
		, _ssl_context( SSLv23_client_method() )
	{
	}
	
	virtual bool condition( tcode::io::ip::socket_type h , const tcode::io::ip::address& addr ){
		return true;
	}

	virtual void on_error( const std::error_code& ec ){
		LOG_D("ECHO" , "acceptor_impl error : %s" , ec.message().c_str());
	}

	virtual bool build( tcode::transport::tcp::pipeline& p ) {
		p.add(new tcode::transport::tcp::ssl_filter( _ssl_context.impl()
			, tcode::transport::tcp::ssl_filter::HANDSHAKE::CONNECT ));
		p.add( new echo_filter());
		return true;
	}
	
	tcode::rc_ptr<tcode::transport::tcp::connector >& connector( void ){
		return _connector;
	}

	virtual tcode::transport::event_loop& channel_loop( void ){	
		return _connector->loop();
	}
private:
	tcode::rc_ptr<tcode::transport::tcp::connector > _connector;
	tcode::ssl::context _ssl_context;
};

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);	

	tcode::ssl::openssl_init();
	
	tcode::transport::event_loop loop;
	connector_impl* conn = new connector_impl( loop );
	tcode::transport::tcp::pipeline_builder_ptr builder(conn);
	if ( conn->connector()->connect(  tcode::io::ip::address::from( "127.0.0.1" , 7543)  , builder , tcode::time_span::minutes(2) )){
		loop.run();
	}
	return 0;
}


