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


class echo_filter 
	: public tcode::transport::tcp::filter
{
public:
	echo_filter( void ){
	}
	virtual ~echo_filter( void ){
	}
	virtual void filter_on_open( const tcode::io::ip::address& addr ){
		std::cout <<"filter_on_open" << std::endl;
	}
	virtual void filter_on_close( void ){
		std::cout <<"filter_on_close" << std::endl;
	}
	virtual void filter_on_read( tcode::buffer::byte_buffer buf ){
		std::cout <<"filter_on_read" << std::endl;
		fire_filter_do_write( buf );
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
int _tmain(int argc, _TCHAR* argv[])
#else
int main( int argc , char* argv[])
#endif
{
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);	
	
	tcode::transport::event_loop loop;
	acceptor_impl acceptor( loop );

	loop.links_add();

	if ( acceptor.listen( tcode::io::ip::address::any( 7543  , AF_INET ) , &acceptor )){
		loop.run();
	}

	return 0;
}


