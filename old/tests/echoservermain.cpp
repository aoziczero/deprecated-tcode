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
#endif

#include <transport/event_loop.hpp>
#include <transport/tcp/acceptor.hpp>
#include <transport/tcp/filter.hpp>
#include <transport/tcp/pipeline_builder.hpp>
#include <transport/tcp/pipeline.hpp>
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

class acceptor_impl : public tcode::transport::tcp::acceptor
	, public tcode::transport::tcp::pipeline_builder
{
public:
	acceptor_impl( tcode::transport::event_loop& l ) 
		: acceptor( l ){
	}
	
	virtual bool condition( tcode::io::ip::socket_type h , const tcode::io::ip::address& addr ){
		return true;
	}

	virtual void on_error( const std::error_code& ec ){
	
	}

	virtual bool build( tcode::transport::tcp::pipeline& p ) {
		p.add( new echo_filter());
		return true;
	}

	virtual tcode::transport::event_loop& channel_loop( void ){	
		return loop();
	}
};


#if defined ( TCODE_TARGET_WINDOWS )
int _tmain(int argc, _TCHAR* argv[]) {
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);	
#else
int main( int argc , char* argv[]) {
#endif		
	tcode::diagnostics::log::logger::instance().add_writer( 
		tcode::diagnostics::log::writer_ptr(new tcode::diagnostics::log::file_writer())
	);
	LOG_T("TAG" , "Start %s" , "test" );
	LOG_D("TAG" , "Start %s" , "test" );
	LOG_I("TAG" , "Start %s" , "test" );
	LOG_W("TAG" , "Start %s" , "test" );
	LOG_E("TAG" , "Start %s" , "test" );
	LOG_F("TAG" , "Start %s" , "test" );

	char* test = "Call My Name";
	LOG_DUMP_T( "tag" , test , strlen(test) , "Test DUMP %s" , "Dump!" );
	
	tcode::transport::event_loop loop;
	acceptor_impl* acceptor =  new acceptor_impl( loop );


	loop.links_add();
	tcode::transport::tcp::pipeline_builder_ptr builder(acceptor);
	if ( acceptor->listen( tcode::io::ip::address::any( 7543  , AF_INET ) , builder )){
		loop.run();
	}
	return 0;
}


