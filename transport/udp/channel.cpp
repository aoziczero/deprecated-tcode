#include "stdafx.h"
#include "channel.hpp"
#include "pipeline.hpp"
#include <transport/event_loop.hpp>
#include <threading/atomic.hpp>
#if defined (TCODE_TARGET_WINDOWS)
#include "completion_handler_recv_from.hpp"
#else
#include <io/io.hpp>
#include <diagnostics/posix_category_impl.hpp>
#endif

#include "stdafx.h"

namespace tcode{ namespace transport { 
namespace udp{ 
namespace detail {
	
const int NO_ERROR_FLAG = 0x10000000;
const int NOT_CLOSED_FLAG = NO_ERROR_FLAG << 1;

}

int channel_config::READ_BUFFER_SIZE = 4096;

channel::channel( event_loop& l
				, const udp::pipeline& p 
				, tcode::io::ip::socket_type fd  )
	: _loop( l )
	, _pipeline(p)
{
	handle( fd );	
	_flag.store( detail::NO_ERROR_FLAG | detail::NOT_CLOSED_FLAG );
	_pipeline.set_channel( this );
	_loop.links_add();
}

channel::~channel( void ) {
	_loop.links_release(); 
}

event_loop& channel::loop( void ){
	return _loop;
}


udp::pipeline& channel::pipeline( void ){
	return _pipeline;
}

void channel::close( void ){
	close( tcode::diagnostics::cancel );
}


void channel::close( const std::error_code& ec ){
	if ( tcode::threading::atomic_bit_reset( _flag , detail::NOT_CLOSED_FLAG ) ) {
		_loop.execute([this , ec ]{
			fire_on_close( ec);
		});
	}
}

void channel::fire_on_close( const std::error_code& ec  ){
	_loop.dispatcher().unbind(
#if defined( TCODE_TARGET_WINDOWS )	
		(HANDLE)handle()
#elif defined (TCODE_TARGET_LINUX)
		handle()
#endif	
	);
	tcode::io::ip::udp_base::close();	
	
	if ( tcode::threading::atomic_bit_reset( _flag , detail::NO_ERROR_FLAG )) {
		_pipeline.fire_filter_on_error( ec );
	}
	_pipeline.fire_filter_on_close();
	release();
}

void channel::add_ref( void ){
	_flag.fetch_add(1);
}

void channel::release( void ){
	int val = _flag.fetch_sub(1
			, std::memory_order::memory_order_release ) - 1;
	if ( val == 0 ) {
		_loop.execute([this]{
			fire_on_end_reference();
		});
	}
}

void channel::fire_on_open(){
	add_ref();
	if ( _loop.in_event_loop() ){
#if defined( TCODE_TARGET_WINDOWS )
		_loop.dispatcher().bind( reinterpret_cast<HANDLE>(handle()));
		_pipeline.fire_filter_on_open();
		read(nullptr);	
#elif defined( TCODE_TARGET_LINUX ) 
		_pipeline.fire_filter_on_open();
		if ( _loop.dispatcher().bind( handle()
			, EPOLLIN
			, this )){
		} else {
			close( tcode::diagnostics::platform_error() );
		}	
#endif
	} else {
		_loop.execute([this]{
			fire_on_open();
		});
	}
}

void channel::fire_on_end_reference( void ){
	_pipeline.fire_filter_on_end_reference();
	delete this;
}

void channel::do_write( const tcode::io::ip::address& addr 
	, tcode::buffer::byte_buffer buf )
{
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG ))
		return;

	if ( !_loop.in_event_loop() ){
		add_ref();
		_loop.execute([this,addr,buf]{
			do_write( addr , buf );
			release();
		});
		return;
	}
	sendto( buf.rd_ptr() , buf.length() , addr );
}


#if defined( TCODE_TARGET_WINDOWS )
void channel::read( completion_handler_recv_from* h ) {
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		if ( h )  delete h;
		return;
	}
		
	if ( h == nullptr ) h = new completion_handler_recv_from( *this );
		
	h->prepare();
	tcode::iovec iov = h->read_buffer( channel_config::READ_BUFFER_SIZE );
	DWORD flag = 0;
    if ( WSARecvFrom( handle() 
                    , &iov
                    , 1
                    , nullptr 
                    , &flag 
					, h->address().sockaddr()
					, h->address().sockaddr_length_ptr()
					, h
                    , nullptr ) == SOCKET_ERROR )
    {
        std::error_code ec = tcode::diagnostics::platform_error();
        if ( ec.value() != WSA_IO_PENDING ){
			h->error_code(ec);
			_loop.execute_handler( h );
        }
    }
}

void channel::handle_recv_from( const tcode::diagnostics::error_code& ec 
			, tcode::buffer::byte_buffer& readbuf
			, const tcode::io::ip::address& addr 			
			, completion_handler_recv_from* h )
{
	if ( tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		if ( ec ){
			close(ec);
		} else {
			_pipeline.fire_filter_on_read( readbuf ,  addr );
			read(h);
			return;
		}
	}
	delete h;
}

	
#elif defined( TCODE_TARGET_LINUX )
void channel::operator()( const int events ){
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		return;
	}
	if ( events & ( EPOLLERR | EPOLLHUP )) {
		if ( tcode::threading::atomic_bit_reset( _flag , detail::NOT_CLOSED_FLAG ) ) {
			fire_on_close( events & EPOLLERR ? tcode::diagnostics::epoll_err : tcode::diagnostics::epoll_hup);
		}
	} else {
		if ( events & EPOLLIN ) {
			handle_read();
		}
	}
}

int channel::read( iovec* iov , int cnt , tcode::io::ip::address& addr ) {
	int result = -1;
	do{
		result = recvfrom( iov->iov_base , iov->iov_len , addr );
	} while( result < 0 &&  errno == EINTR  );
	return result;
}

void channel::handle_read( void ){
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		return;
	}

	tcode::buffer::byte_buffer buffer( channel_config::READ_BUFFER_SIZE );	
	iovec vec = tcode::io::read_buffer( buffer );
	tcode::io::ip::address address;
	
	int result = read( &vec , 1  , address );
	if ( result < 0 ){
		close( tcode::diagnostics::platform_error() );
	} else if ( result == 0 ) {
		close( std::error_code( ECONNRESET , tcode::diagnostics::posix_category()) );
	} else {
		buffer.wr_ptr( result );
		_pipeline.fire_filter_on_read( buffer , address );
	}	
}

#endif

}}}