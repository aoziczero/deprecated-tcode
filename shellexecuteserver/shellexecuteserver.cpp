// SSLEchoServer.cpp : Defines the entry point for the console application.
//


#include <stdint.h>
#include <iostream>
#include <cassert>
#include <cstring>
#include <functional>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iterator>
#include <ctime>
#include <system_error>
#include <common/define.hpp>
#include <diagnostics/tcode_error_code.hpp>

#include <transport/event_loop.hpp>
#include <transport/tcp/acceptor.hpp>
#include <transport/tcp/filter.hpp>
#include <transport/tcp/pipeline_builder.hpp>
#include <transport/tcp/pipeline.hpp>
#include <transport/tcp/filters/size_filter.hpp>
//#include <transport-ssl/openssl.hpp>
//#include <transport-ssl/context.hpp>
//#include <transport-ssl/ssl_filter.hpp>
#include <diagnostics/log/log.hpp>

class shell_execute_filter 
	: public tcode::transport::tcp::filter
{
public:
	shell_execute_filter( void ){
	}
	virtual ~shell_execute_filter( void ){
	}
	virtual void filter_on_read( tcode::buffer::byte_buffer buf ){		
		auto result = exec( (char*)buf.rd_ptr() );
		buf.clear();
		buf.reserve( result.length() + 1 );
		buf.write( const_cast<char*>(result.data()) , result.length() + 1  );
		LOG_D("SHELL" ,  "EXEC %s" , result.c_str());
		fire_filter_do_write( buf );
	}
	std::string exec(char* cmd) {
		FILE* pipe = popen(cmd, "r");
		if (!pipe) return "ERROR";
		char buffer[128];
		std::string result = "";
		while(!feof(pipe)) {
    		if(fgets(buffer, 128, pipe) != NULL)
    			result += buffer;
		}
		pclose(pipe);
		return result;
	}
private:

};

class acceptor_handler_impl 
	: public tcode::transport::tcp::acceptor_handler
{
public:
	acceptor_handler_impl( tcode::transport::event_loop& l ) 
		: _loop( l )
		//, _ssl_context( SSLv23_server_method() )
	{
		//_ssl_context.use_generate_key();
	}
	
	virtual bool condition( tcode::io::ip::socket_type h , const tcode::io::ip::address& addr ){
		return true;
	}

	virtual void acceptor_on_error( const std::error_code& ec ){
		LOG_D("ECHO" , "acceptor_impl error : %s" , ec.message().c_str());
	}

	virtual bool build( tcode::transport::tcp::pipeline& p ) {
		p.add( new tcode::transport::tcp::size_filter());
		//p.add(new tcode::transport::tcp::ssl_filter( _ssl_context.impl() , tcode::transport::tcp::ssl_filter::HANDSHAKE::ACCEPT ));
		p.add( new shell_execute_filter());
		return true;
	}

	virtual tcode::transport::event_loop& channel_loop( void ){	
		return _loop;
	}
private:
	tcode::transport::event_loop& _loop;
	//tcode::ssl::context _ssl_context;
};

int main(int argc, char* argv[])
{
	tcode::diagnostics::log::logger::instance();
	tcode::transport::event_loop loop;
	tcode::transport::tcp::acceptor acceptor(loop);
	tcode::transport::tcp::acceptor_handler_ptr handler( new acceptor_handler_impl(loop));
	if ( acceptor.listen( tcode::io::ip::address::any( 7543  , AF_INET ) , handler )){
		loop.run();
	} else {
		LOG_E("ECHO" , "Listen Fail %s" , tcode::diagnostics::platform_error().message().c_str());
	}
	
	return 0;
}


