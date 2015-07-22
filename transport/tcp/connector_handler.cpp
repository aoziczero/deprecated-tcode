#include "stdafx.h"
#include "connector_handler.hpp"
#include "completion_handler_connect.hpp"
#include "pipeline.hpp"
#include "channel.hpp"

namespace tcode { namespace transport { namespace tcp {
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

connector_handler::connector_handler( void )
	: _channel_loop(nullptr)
	, _current_address_index(0)
	, _timeout( tcode::time_span::minutes(1))
#if defined( TCODE_TARGET_WINDOWS )
	, _completion_handler(nullptr)
#endif
	, _connect_in_progress(false)
{
	
}

connector_handler::~connector_handler( void ){
	
}


tcode::time_span connector_handler::timeout( void ){
	return _timeout;
}

void connector_handler::timeout( const tcode::time_span& timeout ){
	_timeout = timeout;
}

void connector_handler::do_connect(const std::vector< tcode::io::ip::address >& addrs ){	
	connector_handler_ptr ptr(this);	
	_address = addrs;
	_channel_loop = &(channel_loop());
	_channel_loop->links_add();
	_current_address_index = 0;
	_timer = tcode::transport::event_timer::create_timer( *_channel_loop );
	
	_timer->on_expired( 
		[ptr]( const tcode::diagnostics::error_code& ec , const tcode::transport::event_timer& )
		{
			ptr->on_timer(ec);
		});
	do_connect();
	_timer->expired_at( tcode::time_stamp::now() + _timeout );
	_timer->fire();
}

void connector_handler::handle_error( const tcode::diagnostics::error_code& ec ){
	close();
	connector_handler_ptr ptr(this);	
	_channel_loop->execute([ ptr ,ec ] {
		ptr->handle_connect(ec);
	});
}

void connector_handler::do_connect(){
	tcode::io::ip::address& addr = _address[_current_address_index];	
	switch( addr.family() )
	{
	case AF_INET:
		open( AF_INET );
		break;
	case AF_INET6:
		open( AF_INET6 );
		break;
	}
	if ( !good() ) {
		 return handle_error( tcode::diagnostics::platform_error());
	}
	non_blocking non_block;
	if ( !set_option( non_block )){
		return handle_error( tcode::diagnostics::platform_error());
	}	
	if ( !bind( tcode::io::ip::address::any( 0 , addr.family()))){
		return handle_error( tcode::diagnostics::platform_error());
	}	
	_connect_in_progress = do_connect(addr);	
	if ( _connect_in_progress )
		_connect_time = tcode::time_stamp::now();
}

bool connector_handler::do_connect(const tcode::io::ip::address& addr ){
#if defined( TCODE_TARGET_WINDOWS )
	LPFN_CONNECTEX connect_ex = get_connect_ex( handle());	
	if( connect_ex == nullptr ) {
		handle_error( tcode::diagnostics::platform_error());
		return false;
	}

	if ( !_channel_loop->dispatcher().bind( handle())){
		handle_error( tcode::diagnostics::platform_error());
		return false;
	}
	_completion_handler = new completion_handler_connect(this);
	DWORD dwBytes = 0;
	if ( connect_ex( handle() ,  
					addr.sockaddr() , 
					addr.sockaddr_length() , 
					nullptr , 
					0 ,
					&dwBytes ,
					_completion_handler ) == FALSE ) 
	{
		std::error_code ec = tcode::diagnostics::platform_error();
		if ( ec.value() != WSA_IO_PENDING ){
			_completion_handler->error_code( tcode::diagnostics::platform_error() );
			_channel_loop->execute_handler( _completion_handler );
		}
	}	
	return true;
#elif defined( TCODE_TARGET_LINUX ) 
	int r;
	do {
		r = ::connect(handle()
				, addr.sockaddr()
				, addr.sockaddr_length());
	}while((r==-1)&&(errno==EINTR));
	if (( r == 0 ) || errno == EINPROGRESS ) {
		if ( _channel_loop->dispatcher().bind( handle() 
					, EPOLLOUT | EPOLLONESHOT 
					,  this ))
		{
			return true;
		}
	} 
	handle_error( tcode::diagnostics::platform_error());
	return false;
#endif
}

void connector_handler::handle_connect( const tcode::diagnostics::error_code& ec  ){
	_connect_in_progress = false;
	tcode::diagnostics::error_code er = ec;
	if ( !er ) {
		tcode::transport::tcp::pipeline pl;
		if ( build( pl ) ) {
			tcode::transport::tcp::channel* channel 
				= new tcode::transport::tcp::channel( 
						*_channel_loop ,  pl , handle( tcode::io::ip::invalid_socket ));
			channel->fire_on_open(_address[_current_address_index]);
			_timer->cancel();
			return;
		}
		er = tcode::diagnostics::build_fail;
	}
	close();
	connector_on_error(ec , _address[_current_address_index] );
	
	// 리스트의 처음 부터라면 타이머를 통한 처리로 통신부하 감소
	int next = ( _current_address_index + 1 ) % _address.size();
	if ( next == 0 ) 
		return;

	_current_address_index = next;
	if (connector_do_continue(_address[_current_address_index])){		
		do_connect();
	} else {
		_timer->cancel();
	}
}

void connector_handler::on_timer( const tcode::diagnostics::error_code& ec ){
	if ( ec == tcode::diagnostics::cancel ) {
		_timer.reset();
		_channel_loop->links_release();
		return;
	} 
	if ( _connect_in_progress ) {
		if ( tcode::time_stamp::now() > _connect_time + _timeout ){
#if defined( TCODE_TARGET_WINDOWS )
			_completion_handler->cancel();
#elif defined( TCODE_TARGET_LINUX )
			_channel_loop->dispatcher().unbind(handle());				
#endif
			_connect_in_progress = false;
			close();
			connector_on_error(ec , _address[_current_address_index]);
			_current_address_index = ( _current_address_index + 1 ) % _address.size();
			if (connector_do_continue(_address[_current_address_index])){
				do_connect();
			} else {
				_timer.reset();
				_channel_loop->links_release();
				return;
			}	
		}
	} else {
		_current_address_index = ( _current_address_index + 1 ) % _address.size();
		if (connector_do_continue(_address[_current_address_index])){			
			do_connect();
		} else {
			_timer.reset();
			_channel_loop->links_release();
			return;
		}
	}
	_timer->expired_at( tcode::time_stamp::now() + _timeout );
	_timer->fire();
}


#if defined( TCODE_TARGET_LINUX )
void connector_handler::operator()( const int events ){
	if ( events & ( EPOLLERR | EPOLLHUP )) {
		 handle_connect( events & EPOLLERR ? tcode::diagnostics::epoll_err : tcode::diagnostics::epoll_hup );
	} else {
		if ( events & EPOLLOUT ) 
			handle_connect( tcode::diagnostics::success );
	}	
}
#endif

}}}

