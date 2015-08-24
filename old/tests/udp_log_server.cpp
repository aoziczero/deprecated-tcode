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
#include <transport/udp/bootstrap.hpp>
#include <diagnostics/log/log.hpp>

class log_filter 
	: public tcode::transport::udp::filter
{
public:
	log_filter( void ){
	}
	virtual ~log_filter( void ){
	}

	virtual void filter_on_open(  ){
		std::cout <<"filter_on_open" << std::endl;
	}
	virtual void filter_on_close( void ){
		std::cout <<"filter_on_close" << std::endl;
	}
	virtual void filter_on_read( tcode::buffer::byte_buffer buf 
		,const tcode::io::ip::address& addr ){
		tcode::diagnostics::log::record r;
		buf >> r;
		_writer.write(r);
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
	tcode::diagnostics::log::console_writer _writer;
};


class builder 
	: public tcode::transport::udp::pipeline_builder
{
public:
	builder( tcode::transport::event_loop& l ) 
		: _loop( l ){
	}
	
	virtual bool build( tcode::transport::udp::pipeline& p ) {
		p.add( new log_filter());
		return true;
	}

	virtual tcode::transport::event_loop& channel_loop( void ){	
		return _loop;
	}
private:
	tcode::transport::event_loop& _loop;
};



#if defined ( TCODE_TARGET_WINDOWS )
int _tmain(int argc, _TCHAR* argv[]) {
	WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);	
#else
int main( int argc , char* argv[]) {
#endif
	
	tcode::transport::event_loop loop;
	tcode::transport::udp::pipeline_builder_ptr builder( new builder(loop));
	if ( tcode::transport::udp::bootstrap::open_server( tcode::io::ip::address::any( 8888 ) , builder ))
	{
		loop.run();
	}
	
	return 0;
}


