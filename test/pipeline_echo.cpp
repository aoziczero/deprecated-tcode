// tests.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <tcode/io/engine.hpp>
#include <tcode/io/ip/tcp/pipeline/acceptor.hpp>
#include <tcode/io/ip/tcp/pipeline/filter.hpp>
#include <tcode/io/ip/tcp/pipeline/pipeline.hpp>
#include <tcode/log/log.hpp>

class echo_filter 
	: public tcode::io::ip::tcp::filter
{
public:
	echo_filter( void ){
	}
	virtual ~echo_filter( void ){
	}
	virtual void filter_on_open( const tcode::io::ip::address& addr ){
		LOG_T("ECHO" , "filter_on_open %s" , addr.ip().c_str() );
	}
	virtual void filter_on_close( void ){
		LOG_T("ECHO" , "filter_on_close" );
	}
	virtual void filter_on_read( tcode::byte_buffer buf ){
		LOG_T("ECHO" , "filter_on_read %d" , buf.length() );
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

class acceptor_impl : public tcode::io::ip::tcp::pipeline_acceptor
{
public:
	acceptor_impl( tcode::io::engine& l ) 
		: pipeline_acceptor( l )
        , e(l)
    {
	}
	
	virtual bool build( tcode::io::ip::tcp::pipeline& p ) {
        LOG_T("ECHO" , "build");
		p.add( new echo_filter());
		return true;
	}

	virtual tcode::io::engine& engine( void ){	
		return e; 
	}
private:
    tcode::io::engine& e;
};


int main( int argc , char* argv[]) {
	
    LOG_T("ECHO" , "start");
	tcode::io::engine e;
    acceptor_impl a(e);
    a.listen( 7543 );
    e.run();
	return 0;
}


