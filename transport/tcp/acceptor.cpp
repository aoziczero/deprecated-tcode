#include "stdafx.h"
#include "acceptor.hpp"
#include "pipeline.hpp"
#include "pipeline_builder.hpp"
#include "channel.hpp"
#include <diagnostics/log/log.hpp>
#if defined( TCODE_TARGET_WINDOWS )
#include "completion_handler_accept.hpp"
#endif

namespace tcode { namespace transport { namespace tcp {

diagnostics::log::logger& acceptor::logger(){
	static diagnostics::log::logger instance;
	return instance;
}

acceptor::acceptor(tcode::transport::event_loop& l)
	: _loop(l) 
{
	LOGGER_T( logger() , "acceptor" , "create instance" );
}

acceptor::~acceptor(void){
	LOGGER_T( logger() , "acceptor" , "release instance" );
}

bool acceptor::listen( const tcode::io::ip::address& bind_addr
		, const acceptor_handler_ptr& handler  )
{
	_handler = handler;
	switch( bind_addr.family() )
	{
	case AF_INET:
		open( AF_INET );
		break;
	case AF_INET6:
		open( AF_INET6 );
		break;
	default:	
		LOGGER_E( logger() , "acceptor" , "socket fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());
		return false;
	}
	if ( !good() )
		return false;

	reuse_address reuse(true);	
	if ( !set_option( reuse )){
		LOGGER_E( logger() , "acceptor" , "reuse_address fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());
		close();
		return false;
	}	
	non_blocking non_block;
	if ( !set_option( non_block )){
		LOGGER_E( logger() , "acceptor" , "non_blocking fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());		
		close();
		return false;
	}	
	_address_family = bind_addr.family();
#if defined( TCODE_TARGET_WINDOWS )
	conditional_accept cond_accept( TRUE );
	if ( !set_option( cond_accept )){
		LOGGER_E( logger() , "acceptor" , "conditional_accept fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());		
		close();
		return false;
	}
#endif
	if ( !bind( bind_addr )){
		LOGGER_E( logger() , "acceptor" , "bind fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());		
		close();
		return false;
	}
		
	if ( !tcode::io::ip::accept_base::listen()){
		LOGGER_E( logger() , "acceptor" , "listen fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());		
		close();
		return false;
	}
#if defined( TCODE_TARGET_WINDOWS )
	if ( !_loop.dispatcher().bind( handle())){
		LOGGER_E( logger() , "acceptor" , "dispatcher bind fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());		
		close();
		return false;
	}
	do_accept( nullptr );
#elif defined( TCODE_TARGET_LINUX )
	if ( !_loop.dispatcher().bind( handle() , EPOLLIN ,  this )){
		LOGGER_E( logger() , "acceptor" , "dispatcher bind fail : %s"
			 , tcode::diagnostics::platform_error().message().c_str());		
		close();
		return false;
	}
	_loop.links_add();
#endif
	LOGGER_T( logger() , "acceptor" , "listen success!");
	return true;;
}

void acceptor::close( void ){
#if defined( TCODE_TARGET_LINUX )
	_loop.dispatcher().unbind( handle() );
	_loop.links_release();
#endif
	tcode::io::ip::accept_base::close();	
}

tcode::transport::event_loop& acceptor::loop( void ){
	return _loop;
}
	
#if defined( TCODE_TARGET_WINDOWS )
void acceptor::do_accept(completion_handler_accept* h){
	if ( handle() == tcode::io::ip::invalid_socket ) {
		LOGGER_W( logger() , "acceptor" , "handle() == invalid_socket" );		
		if ( h )
			delete h;
		return;
	}
	if ( h == nullptr )
		h = new completion_handler_accept( *this );

	tcode::io::ip::tcp_holder tcp_handle;
	tcp_handle.open( _address_family );
	if ( !tcp_handle.good()) {
		LOGGER_E( logger() , "acceptor" , "create accept socket fail %s" 
			 , tcode::diagnostics::platform_error().message().c_str());	
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
			LOGGER_E( logger() , "acceptor" , "AcceptEx fail %s" , ec.message().c_str());	
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
		LOGGER_D( logger() , "acceptor" , "accept error %s"  , ec.message().c_str());	
		tcp_handle.close();
		_handler->acceptor_on_error( ec );
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
		if ( _handler->condition( tcp_handle.handle() , handler->address_ptr()[1] )){
			tcode::io::ip::tcp_holder::update_accept_context ctx(handle());
			tcp_handle.set_option( ctx );
			tcode::transport::tcp::channel* channel 
					= new tcode::transport::tcp::channel( 
							_handler->channel_loop() ,tcp_handle.handle());
			if ( _handler->build( channel->pipeline() ) ) {
				channel->fire_on_open( handler->address_ptr()[1] );
			} else {
				tcp_handle.close();
				delete channel;
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
		 _handler->acceptor_on_error( events & EPOLLERR ? tcode::diagnostics::epoll_err 
			: tcode::diagnostics::epoll_hup );
	} else {
		if ( events & EPOLLIN ) handle_accept();
	}
}

void acceptor::handle_accept( void ) {
	tcode::io::ip::tcp_holder tcp_handle;
	tcode::io::ip::address addr;
	do {
		tcp_handle.handle(::accept( handle() , addr.sockaddr() , addr.sockaddr_length_ptr() ));
	} while((tcp_handle.handle()==-1)&&(errno==EINTR));

	if ( tcp_handle.handle() < 0 ) {
		tcode::diagnostics::error_code ec(tcode::diagnostics::platform_error());
		_handler->acceptor_on_error( ec );
		LOGGER_D( logger() , "acceptor" , "accept fail %s" , ec.message().c_str());	
		return;
	}

	if ( _handler->condition( tcp_handle.handle() , addr )){
		tcode::io::ip::tcp_holder::non_blocking non_block;
		tcp_handle.set_option( non_block );

		tcode::transport::tcp::channel* channel 
				= new tcode::transport::tcp::channel( 
						_handler->channel_loop() ,tcp_handle.handle());
		if ( _handler->build( channel->pipeline() ) ) {
			channel->fire_on_open( handler->address_ptr()[1] );
		} else {
			tcp_handle.close();
			delete channel;
		}
	} else {
		tcp_handle.close();
	}
}

#endif

}}}