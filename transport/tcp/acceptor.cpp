#include "stdafx.h"
#include "acceptor.hpp"
#include "pipeline.hpp"
#include "pipeline_builder.hpp"
#include "channel.hpp"

namespace tcode { namespace transport { namespace tcp {
#if defined( TCODE_TARGET_WINDOWS )
class completion_handler_accept
	: public tcode::io::completion_handler
{
public:
	completion_handler_accept( acceptor& pacceptor )
		: _acceptor(pacceptor){
	}

	virtual ~completion_handler_accept( void ){
	
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
#endif


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
#if defined( TCODE_TARGET_WINDOWS )
	conditional_accept cond_accept( TRUE );
	if ( !set_option( cond_accept )){
		close();
		return false;
	}
#endif
	if ( !bind( bind_addr )){
		close();
		return false;
	}
		
	if ( !tcode::io::ip::accept_base::listen()){
		close();
		return false;
	}
#if defined( TCODE_TARGET_WINDOWS )
	if ( !_loop.dispatcher().bind( reinterpret_cast<HANDLE>(handle())))
	{
		close();
		return false;
	}
	do_accept( nullptr );
#elif defined( TCODE_TARGET_LINUX )
	if ( !_loop.dispatcher().bind( handle() , EPOLLIN ,  this )){
		close();
		return false;
	}
#endif
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

#if defined( TCODE_TARGET_WINDOWS )
void acceptor::do_accept(completion_handler_accept* h){
	if ( handle() == tcode::io::ip::invalid_socket ) {
		if ( h )
			delete h;
		return;
	}
	if ( h == nullptr ) {
		h = new completion_handler_accept( *this );
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
				, completion_handler_accept* handler  )
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
#elif defined( TCODE_TARGET_LINUX )

void acceptor::operator()( const int events ){
	if ( events & ( EPOLLERR | EPOLLHUP )) {
		 on_error( events & EPOLLERR ? tcode::diagnostics::epoll_err 
					: tcode::diagnostics::epoll_hup );
	} else {
		if ( events & EPOLLIN ) {
			handle_accept();
		} 
	}
}
void acceptor::handle_accept( void ) {
	tcode::io::ip::tcp_holder fd;
	tcode::io::ip::address addr;
	do {
		fd.handle(::accept( handle()
				, addr.sockaddr()
				, addr.sockaddr_length_ptr() ));
	} while((fd.handle()==-1)&&(errno==EINTR));

	if ( fd.handle() < 0 ) {
		return;
	}

	if ( condition( fd.handle() , addr )){
		tcode::io::ip::tcp_holder::non_blocking non_block;
		fd.set_option( non_block );
		tcode::transport::tcp::pipeline pl;
		if ( _builder && _builder->build( pl ) ) {
			tcode::transport::tcp::channel* channel 
					= new tcode::transport::tcp::channel( 
							_builder->channel_loop() , pl ,fd.handle());
				channel->fire_on_open( addr );
		} else {
			fd.close();
		}
	} else {
		fd.close();
	}
}

#endif

}}}