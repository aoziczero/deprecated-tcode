#include "stdafx.h"
#include "acceptor.hpp"
#include "pipeline.hpp"
#include "pipeline_builder.hpp"
#include "channel.hpp"

namespace tcode { namespace transport { namespace tcp {

class accept_completion_handler
	: public tcode::io::completion_handler
{
public:
	accept_completion_handler( acceptor& pacceptor )
		: _acceptor(pacceptor){
	}

	virtual ~accept_completion_handler( void ){
	
	}
	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes )
	{
		_acceptor.handle_accept( ec , this );
	}

	SOCKET handle( void ){
		return _handle;
	}

	void handle( SOCKET s ){
		_handle = s;
	}
		
	tcode::io::ip::address* address_ptr( void ) {
		return _address;
	}
private:
	acceptor& _acceptor;
	SOCKET _handle;
	tcode::io::ip::address _address[2];
};



acceptor::acceptor(tcode::transport::event_loop& l)
	: _loop(l) 
{

}

acceptor::~acceptor(void){
}

bool acceptor::listen( const tcode::io::ip::address& bind_addr
	, pipeline_builder* builder )
{
	_builder = builder;
	switch( bind_addr.family() )
	{
	case AF_INET:
		open( AF_INET );
		break;
	case AF_INET6:
		open( AF_INET6 );
		break;
	default:
		return false;
	}
	if ( !good() ) {
		return false;
	}
	reuse_address reuse(true);	
	if ( !set_option( reuse )){
		close();
		return false;
	}	
	non_blocking non_block;
	if ( !set_option( non_block )){
		close();
		return false;
	}	
	_address_family = bind_addr.family();
	conditional_accept cond_accept( TRUE );
	if ( !set_option( cond_accept )){
		close();
		return false;
	}
	if ( !bind( bind_addr )){
		close();
		return false;
	}
		
	if ( !tcode::io::ip::accept_base::listen()){
		close();
		return false;
	}

	if ( !_loop.dispatcher().bind( reinterpret_cast<HANDLE>(handle())))
	{
		close();
		return false;
	}
	do_accept( nullptr );
	return true;;
}

tcode::transport::event_loop& acceptor::loop( void ){
	return _loop;
}
	
bool acceptor::condition( tcode::io::ip::socket_type h , const tcode::io::ip::address& addr )
{
	return true;
}

void acceptor::on_error( const std::error_code& ec ){
	
}

void acceptor::do_accept(accept_completion_handler* h){
	if ( handle() == tcode::io::ip::invalid_socket ) 
		return;
	if ( h == nullptr ) {
		h = new accept_completion_handler( *this );
	}
	tcode::io::ip::tcp_holder tcp_handle;
	tcp_handle.open( _address_family );
	if ( !tcp_handle.good()) {
		h->error_code( tcode::diagnostics::platform_error() );
		_loop.execute_handler( h );
		return;
	}

	tcode::io::ip::tcp_holder::non_blocking non_block;
	tcp_handle.set_option( non_block );

	h->handle( tcp_handle.handle());
	h->prepare();
    DWORD dwBytes = 0;
    if ( AcceptEx(  handle()
					, h->handle()
					, h->address_ptr()
					, 0
					, sizeof( sockaddr_storage )
					, sizeof( sockaddr_storage )
					, &dwBytes
					, h ) == FALSE )
    {
        std::error_code ec = tcode::diagnostics::platform_error();
        if ( ec.value() != WSA_IO_PENDING ){
			h->error_code( tcode::diagnostics::platform_error() );
			_loop.execute_handler( h );
	    }
    }
}

void acceptor::handle_accept( const tcode::diagnostics::error_code& ec 
				, accept_completion_handler* handler  )
{
	tcode::io::ip::tcp_holder tcp_handle;
	tcp_handle.handle( handler->handle());
	if ( ec ){
		tcp_handle.close();
		on_error( ec );
	} else {
		struct sockaddr* remote , *local;
		INT remotelen , locallen;
		GetAcceptExSockaddrs(	handler->address_ptr()  ,
								0 ,
								sizeof( sockaddr_storage ) ,
								sizeof( sockaddr_storage ) ,
								&local ,
								&locallen ,
								&remote ,
								&remotelen );
		if ( condition( tcp_handle.handle() , handler->address_ptr()[1] )){
			tcode::io::ip::tcp_holder::update_accept_context ctx(handle());
			tcp_handle.set_option( ctx );
			tcode::transport::tcp::pipeline pl;
			if ( _builder && _builder->build( pl ) ) {
				tcode::transport::tcp::channel* channel 
					= new tcode::transport::tcp::channel( 
							_builder->channel_loop() ,pl ,tcp_handle.handle());
				channel->fire_on_open( handler->address_ptr()[1] );
			} else {
				tcp_handle.close();
			}
		} else {
			tcp_handle.close();
		}
	}
	do_accept(handler);
}


}}}