#include "stdafx.h"
#include "completion_handler_connect.hpp"
#include "channel.hpp"
#include <transport/event_loop.hpp>
#include <diagnostics/windows_category_impl.hpp>
#include <threading/atomic.hpp>

namespace tcode { namespace transport { namespace tcp {
/*
#if defined(TCODE_TARGET_WINDOWS)	
LPFN_CONNECTEX get_connect_ex( SOCKET s ){
	LPFN_CONNECTEX conn = nullptr ;
	DWORD dwbytes = 0;
	GUID guid = WSAID_CONNECTEX;
	WSAIoctl( s ,
			SIO_GET_EXTENSION_FUNCTION_POINTER , 
			&guid , 
			sizeof( GUID ) , 
			&conn , 
			sizeof( LPFN_CONNECTEX ) , 
			&dwbytes,
			nullptr , 
			nullptr );
	return conn;
}

#endif

completion_handler_connect::completion_handler_connect( 
	tcode::transport::event_loop& l )
	: _loop(l) , _builder(nullptr) 
{
	_flag.store(1);
}

completion_handler_connect::~completion_handler_connect( void ){

}	

void completion_handler_connect::connect( 
		  const tcode::io::ip::address& conn_addr
		, pipeline_builder_ptr builder 
		, connector::event_handler_ptr ptr
		, const tcode::time_span& ts )
{
	// callback ¿ë ref
	add_ref();
	_builder = builder;
	_conn_addr = conn_addr;
	_event_handler = ptr;
	if (!_handle.open( _conn_addr.family())) {
		error_code( tcode::diagnostics::platform_error() );
		_loop.execute_handler( this );
		return;
	}
	tcode::io::ip::tcp_base::non_blocking nb;
	_handle.set_option( nb );	
#if defined(TCODE_TARGET_WINDOWS)	
	if (! _handle.bind( tcode::io::ip::address::any( 0 , _conn_addr.family()))){
		error_code( tcode::diagnostics::platform_error() );
		_loop.execute_handler( this );
		return;
	}
	LPFN_CONNECTEX connect_ex = get_connect_ex( _handle.handle());
	if ( connect_ex == nullptr ) {
		error_code( tcode::diagnostics::platform_error() );
		_loop.execute_handler( this );
		return;
	}
	if ( !_loop.dispatcher().bind( reinterpret_cast<HANDLE>(_handle.handle()))){
		error_code( tcode::diagnostics::platform_error() );
		_loop.execute_handler( this );
		return;
	}	
	DWORD dwBytes = 0;
	if ( connect_ex( _handle.handle() ,  
					_conn_addr.sockaddr() , 
					_conn_addr.sockaddr_length() , 
					nullptr , 
					0 ,
					&dwBytes ,
					this ) == FALSE ) 
	{
		std::error_code ec = tcode::diagnostics::platform_error();
		if ( ec.value() != WSA_IO_PENDING ){
			error_code( tcode::diagnostics::platform_error() );
			_loop.execute_handler( this );
			return;
		}
	}
#elif defined( TCODE_TARGET_LINUX ) 
	if ( !do_connect() ) {
		error_code( tcode::diagnostics::platform_error() );
		_loop.execute_handler( this );
	}
	if ( !_loop.dispatcher().bind( _handle.handle()
					, EPOLLOUT | EPOLLONESHOT 
					,  this ))
	{
		error_code( tcode::diagnostics::platform_error() );
		_loop.execute_handler( this );
	}
#endif
	tcode::rc_ptr< completion_handler_connect > hold(this);
	_timer = tcode::transport::event_timer::create( _loop );
	_timer->expired_at( tcode::time_stamp::now() + ts );
	_timer->on_expired( 
		tcode::transport::event_timer::handler(
			[hold] ( const tcode::diagnostics::error_code& ec , const event_timer& )
			{
				if ( ec != tcode::diagnostics::cancel )
					hold->on_timer();
			}));
	_timer->fire();
}

void completion_handler_connect::on_timer( void ){
	if ( tcode::threading::atomic_bit_reset( _flag , 1 ) ){
		handle_connect(tcode::diagnostics::timeout);
	}
}

void completion_handler_connect::cancel( void ){
	if ( tcode::threading::atomic_bit_reset( _flag , 1 ) ){
		tcode::rc_ptr< completion_handler_connect > hold(this);
		_loop.execute( [hold , this ]{
			_handle.close();
			_timer->cancel();
#if defined(TCODE_TARGET_LINUX)	
			release();
#endif
		});
	}	
}

void completion_handler_connect::handle_connect( const tcode::diagnostics::error_code& ec ){
	_timer->cancel();
	if (ec){
		_handle.close();
		_event_handler->on_error( ec );
		return;
	} 
	tcode::transport::tcp::pipeline pl;
	if ( _builder && _builder->build( pl ) ) {
		_event_handler->on_connect(_conn_addr);
		tcode::transport::tcp::channel* channel 
				= new tcode::transport::tcp::channel( 
						_builder->channel_loop() , pl ,_handle.handle());
		channel->fire_on_open( _conn_addr );
		return;
	}
	_handle.close();
	_event_handler->on_error( tcode::diagnostics::build_fail );
	//	
}

void completion_handler_connect::operator()( const tcode::diagnostics::error_code& ec 
		, const int completion_bytes )
{
	if ( tcode::threading::atomic_bit_reset( _flag , 1 ) ){
		handle_connect(ec);
	}
	release();
}

#if defined( TCODE_TARGET_LINUX )
void completion_handler_connect::operator()( const int events )
{
	if ( tcode::threading::atomic_bit_reset( _flag , 1 ) ){
		if ( events & ( EPOLLERR | EPOLLHUP )) {
			handle_connect( events & EPOLLERR ? tcode::diagnostics::epoll_err : tcode::diagnostics::epoll_hup );
		} else {
			if ( events & EPOLLOUT ) {
				handle_connect( tcode::diagnostics::error_code() );
			}
		}
		release();
	}
}
#endif
*/

}}}

