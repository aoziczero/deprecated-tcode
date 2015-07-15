// tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#if defined ( TCODE_TARGET_WINDOWS )
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "gtestd-mtdd")
#pragma comment(lib, "tcode.common.lib")
#pragma comment(lib, "tcode.threading.lib")
#pragma comment(lib, "tcode.buffer.lib")
#pragma comment(lib, "tcode.diagnostics.lib")
#pragma comment(lib, "tcode.io.lib")
#pragma comment(lib, "tcode.transport.lib")
#pragma comment(lib, "tcode.transport-ssl.lib")
#endif

#include <transport/event_loop.hpp>
#include <transport/tcp/connector.hpp>
#include <transport/tcp/filter.hpp>
#include <transport/tcp/pipeline_builder.hpp>
#include <transport/tcp/pipeline.hpp>


#include <io/ip/resolver.hpp>

#include <diagnostics/log/log.hpp>

class test_http_get_filter : public tcode::transport::tcp::filter {
public:
	virtual void filter_on_open( const tcode::io::ip::address& addr ){
		tcode::buffer::byte_buffer buf(256);
		buf.write_fmt( "GET /index.htm HTTP/1.1\r\n\r\n" );
		fire_filter_do_write( buf );
	}
	virtual void filter_on_close( void ){
		std::cout <<"filter_on_close" << std::endl;
	}
	virtual void filter_on_read( tcode::buffer::byte_buffer buf ){
		buf << '\0';
		std::cout <<"on_read : %s " << buf.rd_ptr() << std::endl;
	}
	virtual void filter_on_write( int written , bool flush ){
		std::cout <<"filter_on_write" << std::endl;
	}
	virtual void filter_on_error( const std::error_code& ec ){
		std::cout <<"filter_on_error" << std::endl;
	}
	virtual void filter_on_end_reference( void ){
		std::cout <<"filter_on_end_reference" << std::endl;
		delete this;
	}
private:
};

class connector_impl : public tcode::transport::tcp::connector::error_handler
	, public tcode::transport::tcp::pipeline_builder
{
public:
	connector_impl( tcode::transport::event_loop& l ) {
		_connector = tcode::transport::tcp::connector::create( l , 
			tcode::rc_ptr< tcode::transport::tcp::connector::error_handler >(this)
		  );
	}
	

	virtual void operator()( const std::error_code& ec ){
	
	}

	virtual bool build( tcode::transport::tcp::pipeline& p ) {
		p.add( new test_http_get_filter());
		return true;
	}

	virtual tcode::transport::event_loop& channel_loop( void ){
		return _connector->loop();
	}
	const tcode::rc_ptr< tcode::transport::tcp::connector >& connector() {	
		return _connector;
	}
private:
	tcode::rc_ptr< tcode::transport::tcp::connector > _connector;
};

#if defined ( TCODE_TARGET_WINDOWS )
int _tmain(int argc, _TCHAR* argv[]) {
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);	
#else
int main( int argc , char* argv[]) {
#endif
	tcode::diagnostics::log::formatter_ptr formatter( new tcode::diagnostics::log::serialize_formatter());
	tcode::diagnostics::log::writer_ptr writer(new tcode::diagnostics::log::udp_writer( 
			tcode::io::ip::address::from( "127.0.0.1" , 8888 )
		));
	writer->formatter(formatter);
	tcode::diagnostics::log::logger::instance().add_writer( writer );
	
	LOG_T("TAG" , "Start %s" , "test" );
	LOG_D("TAG" , "Start %s" , "test" );
	LOG_I("TAG" , "Start %s" , "test" );
	LOG_W("TAG" , "Start %s" , "test" );
	LOG_E("TAG" , "Start %s" , "test" );
	LOG_F("TAG" , "Start %s" , "test" );

	char* test = "Call My Name";
	LOG_DUMP_T( "tag" , test , strlen(test) , "Test DUMP %s" , "Dump!" );
	tcode::transport::event_loop loop;
	connector_impl* conn = new connector_impl( loop );
	tcode::io::ip::resolver r;
	std::vector< tcode::io::ip::address > addrs = r.resolve( "google.co.kr" , 80 , AF_INET );
	tcode::transport::tcp::pipeline_builder_ptr builder( conn );
	if ( conn->connector()->connect( addrs[0] , builder , tcode::time_span::minutes(2) )){
		loop.run();
	}
	return 0;
}




