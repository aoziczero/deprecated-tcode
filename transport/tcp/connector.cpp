#include "stdafx.h"
#include "connector.hpp"

#include "pipeline.hpp"
#include "channel.hpp"
#include "completion_handler_connect.hpp"

namespace tcode { namespace transport { namespace tcp {

connector::connector(void){

}

connector::~connector(void){

}

void connector::connect( const tcode::io::ip::address& addr
		, const tcode::time_span& wait_time 
		, const connector_handler_ptr handler )
{
	std::vector< tcode::io::ip::address > addrs;
	addrs.push_back(addr);
	connect_sequence( addrs , wait_time , handler );
}

void connector::connect_sequence( const std::vector< tcode::io::ip::address >& addr 
		, const tcode::time_span& per_wait_time 
		, const connector_handler_ptr handler )
{
	handler->timeout(per_wait_time);
	handler->do_connect( addr );
}


/*
#if defined(TCODE_TARGET_WINDOWS)
LPFN_CONNECTEX fp_connect_ex = nullptr;
#endif
connector::connector(tcode::transport::event_loop& l )
	: _loop(l) 
	, _timeout(false)
{

}

connector::~connector(void){

}

bool connector::connect_timeout( const tcode::io::ip::address& conn_addr
		, const connector_handler_ptr builder 
		, const tcode::time_span& ts )
{
	tcode::io::ip::connect_base cb;
	if (cb.open( conn_addr.family())){
		tcode::io::ip::connect_base::non_blocking non_block;
		cb.set_option( non_block );
		if ( cb.connect( conn_addr , ts )){
			tcode::transport::tcp::pipeline pl;
			if ( builder && builder->build( pl ) ) {
				tcode::transport::tcp::channel* channel 
						= new tcode::transport::tcp::channel( 
								builder->channel_loop() , pl ,cb.handle());
				channel->fire_on_open( conn_addr );
				return true;;
			} 
		}
	}
	cb.close();
	return false;
}

bool connector::connect( const tcode::io::ip::address& addr
	, const connector_handler_ptr builder 
	, const tcode::time_span& ts  )
{
	_address = addr;
	_builder = builder;	
	switch( addr.family() )
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
	non_blocking non_block;
	if ( !set_option( non_block )){
		close();
		return false;
	}	
#if defined(TCODE_TARGET_WINDOWS)	
	if ( !bind( tcode::io::ip::address::any( 0 , _address.family()))){
		close();
		return false;
	}

	if ( fp_connect_ex == nullptr ){
		DWORD dwbytes = 0;
		GUID guid = WSAID_CONNECTEX;
		if ( ( WSAIoctl(   
				handle() ,
				SIO_GET_EXTENSION_FUNCTION_POINTER , 
				&guid , 
				sizeof( GUID ) , 
				&fp_connect_ex , 
				sizeof( LPFN_CONNECTEX ) , 
				&dwbytes,
				nullptr , 
				nullptr ) == SOCKET_ERROR ) 
			|| ( fp_connect_ex == nullptr )) 
		{	
			close();
			return false;
		}
	}		

	if ( !_loop.dispatcher().bind( handle()))
	{
		close();
		return false;
	}
#endif
	if ( do_connect() ) {
		tcode::rc_ptr< connector > pthis(this);
		_timer = event_timer::create_timer( loop() );
		_timer->expired_at( tcode::time_stamp::now() + ts );
		_timer->on_expired(event_timer::handler(
				[this , pthis ]( const tcode::diagnostics::error_code& ec, const event_timer& ){
					if ( ec != tcode::diagnostics::cancel ) {
						_timeout = true;
	#if defined( TCODE_TARGET_WINDOWS )	
						close();
	#else
						loop().dispatcher().unbind( handle());
						handle_connect( tcode::diagnostics::timeout );
	#endif					
					}
				}));
		_timer->fire();
		return true;
	}
	return false;
}

tcode::transport::event_loop& connector::loop( void ){
	return _loop;
}


bool connector::do_connect(void){
	
#if defined( TCODE_TARGET_WINDOWS )
	this->add_ref();
	this->prepare();
    DWORD dwBytes = 0;
	if ( fp_connect_ex( handle() ,  
                        _address.sockaddr() , 
                        _address.sockaddr_length() , 
                        nullptr , 
                        0 ,
                        &dwBytes ,
						this) == FALSE ) 
    {
        std::error_code ec = tcode::diagnostics::platform_error();
        if ( ec.value() != WSA_IO_PENDING ){
            this->error_code( ec );
			_loop.execute_handler(this);
        }
    }
	
	return true;
#elif defined( TCODE_TARGET_LINUX ) 
	int r;
	do {
		r = ::connect(handle()
				, _address.sockaddr()
				, _address.sockaddr_length());
	}while((r==-1)&&(errno==EINTR));
	if (( r == 0 ) || errno == EINPROGRESS ) {
		if ( _loop.dispatcher().bind( handle() 
					, EPOLLOUT | EPOLLONESHOT 
					,  this ))
		{
			this->add_ref();
			return true;	
		}
	} 
	close();
	return false;
#endif
}

void connector::handle_connect( const tcode::diagnostics::error_code& ec )
{
	auto er = ec;
	if ( _timeout )
		er = tcode::diagnostics::timeout;
	if ( !er ){
		tcode::transport::tcp::pipeline pl;
		if ( _builder && _builder->build( pl ) ) {
			tcode::transport::tcp::channel* channel 
				= new tcode::transport::tcp::channel( 
						_builder->channel_loop()
					,  pl
					,	handle( tcode::io::ip::invalid_socket ));
			channel->fire_on_open(_address);
			return;
		} else {
			er = tcode::diagnostics::build_fail;
		}
	}
	_builder->connector_on_error(er);
	tcode::io::ip::connect_base::close();
	this->release();
}

#if defined( TCODE_TARGET_WINDOWS )

void connector::operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes )
{	
	_timer->cancel();
	handle_connect(ec);
}

#elif defined( TCODE_TARGET_LINUX )

void connector::operator()( const int events )
{		
	_timer->cancel();
	if ( events & ( EPOLLERR | EPOLLHUP )) {
		 handle_connect( events & EPOLLERR ? tcode::diagnostics::epoll_err : tcode::diagnostics::epoll_hup );
	} else {
		if ( events & EPOLLOUT ) {
			handle_connect( tcode::diagnostics::success );
		}
	}	
}

#endif
*/
}}}




/*
connector::connector(tcode::transport::event_loop& l)
	: _loop( l )
{

}

connector::~connector(void) {
}

bool connector::connect_timeout( const tcode::io::ip::address& conn_addr
		, pipeline_builder_ptr builder 
		, const tcode::time_span& ts )
{
	tcode::io::ip::connect_base cb;
	if (cb.open( conn_addr.family())){
		tcode::io::ip::connect_base::non_blocking non_block;
		cb.set_option( non_block );
		if ( cb.connect( conn_addr , ts )){
			auto handle = cb.handle();
			_loop.execute( [handle,conn_addr,builder]{
				tcode::transport::tcp::pipeline pl;
				if ( builder && builder->build( pl ) ) {
					tcode::transport::tcp::channel* channel 
							= new tcode::transport::tcp::channel( 
									builder->channel_loop() , pl ,handle);
					channel->fire_on_open( conn_addr );
				} else {
					tcode::io::ip::tcp_holder h;
					h.handle( handle );
					h.close();
				}
			});
		}
	}
	cb.close();
	return false;
}

connect_id connector::connect(const tcode::io::ip::address& conn_addr
		, pipeline_builder_ptr builder 
		, event_handler_ptr handler
		, const tcode::time_span& ts )
{
	connect_id ci( new completion_handler_connect(_loop));
	ci->connect( conn_addr  , builder , handler , ts);
	return ci;
}

*/

