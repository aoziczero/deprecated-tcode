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
#include <transport/tcp/filters/size_filter.hpp>
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


class test_shell_run_filter 
	: public tcode::transport::tcp::filter
{
public:
	test_shell_run_filter( void ){
	}
	virtual ~test_shell_run_filter( void ){
	}
	virtual void filter_on_open( const tcode::io::ip::address& addr ){
		LOG_T("ECHO" , "filter_on_open %s" , addr.ip_address().c_str() );
		tcode::buffer::byte_buffer hello(10);
		hello.write_msg("arp\0");
		fire_filter_do_write(hello);
	}
	
	virtual void filter_on_read( tcode::buffer::byte_buffer buf ){
		LOG_T("ECHO" , "filter_on_read:\n %s" , buf.rd_ptr() );
	}
	
private:

};

class connector_handler_impl 
	: public tcode::transport::tcp::connector_handler {
public:
	connector_handler_impl( tcode::transport::event_loop& l ) 
		: _loop( l )
		//, _ssl_context( SSLv23_client_method() )
	{
	}

	virtual void connector_on_error( const std::error_code& ec 
		, const tcode::io::ip::address& addr  ){
		LOG_D("ECHO" , "acceptor_impl error : %s : %s" , addr.ip_address().c_str() , ec.message().c_str());
	}

	virtual bool connector_do_continue( const tcode::io::ip::address& addr ){
		return true;
	}

	virtual bool build( tcode::transport::tcp::pipeline& p ) {
		p.add( new tcode::transport::tcp::size_filter());
		//p.add(new tcode::transport::tcp::ssl_filter( _ssl_context.impl() , tcode::transport::tcp::ssl_filter::HANDSHAKE::CONNECT ));
		p.add( new test_shell_run_filter());
		return true;
	}

	virtual tcode::transport::event_loop& channel_loop( void ){	
		return _loop;
	}
private:
	tcode::transport::event_loop& _loop;
	//tcode::ssl::context _ssl_context;
};

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);	
	tcode::ssl::openssl_init();
	tcode::transport::event_loop loop;
	tcode::transport::tcp::connector conn;
	tcode::transport::tcp::connector_handler_ptr handler( new connector_handler_impl( loop ));
	std::vector< tcode::io::ip::address > addrs;
	//addrs.push_back(tcode::io::ip::address::from( "127.0.0.1" , 7543));
	addrs.push_back(tcode::io::ip::address::from( "192.168.0.222" , 7543));
	conn.connect_sequence( addrs , tcode::time_span::seconds(10) , handler );
	loop.run();
	return 0;
}



