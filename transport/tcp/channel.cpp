#include "stdafx.h"
#include "channel.hpp"
#include "pipeline.hpp"
#include <transport/event_loop.hpp>
#include <threading/atomic.hpp>
#if defined (TCODE_TARGET_WINDOWS)
#include "completion_handler_read.hpp"
#include "completion_handler_write.hpp"
#else
#include <io/io.hpp>
#include <diagnostics/posix_category_impl.hpp>
#endif
#include "channel_config.hpp"


namespace tcode{ namespace transport { 
namespace tcp{ 
namespace detail {
	
const int NO_ERROR_FLAG = 0x10000000;
const int NOT_CLOSED_FLAG = NO_ERROR_FLAG << 1;

}


channel::channel( event_loop& l
				, const tcp::pipeline& p 
				, tcode::io::ip::socket_type fd  )
	: _loop( l )
	, _pipeline(p)
{
	handle( fd );	
	_flag.store( detail::NO_ERROR_FLAG | detail::NOT_CLOSED_FLAG );
	_pipeline.channel( this );
	_loop.links_add();
}

channel::~channel( void ) {
	_loop.links_release(); 
}

event_loop& channel::loop( void ){
	return _loop;
}


tcp::pipeline& channel::pipeline( void ){
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
	tcode::io::ip::tcp_base::close();	
	
	if ( tcode::threading::atomic_bit_reset( _flag , detail::NO_ERROR_FLAG )) {
		_pipeline.fire_filter_on_error( ec );
	}
	_pipeline.fire_filter_on_close();
	release();
}

void channel::add_ref( void ){
	_flag.fetch_add(1);
}

int channel::release( void ){
	int val = _flag.fetch_sub(1
			, std::memory_order::memory_order_release ) - 1;
	if ( val == 0 ) {
		_loop.execute([this]{
			fire_on_end_reference();
		});
	}
	return val;
}

void channel::fire_on_open( const tcode::io::ip::address& addr ){
	add_ref();
	if ( _loop.in_event_loop() ){
#if defined( TCODE_TARGET_WINDOWS )
		_loop.dispatcher().bind( reinterpret_cast<HANDLE>(handle()));
		_pipeline.fire_filter_on_open(addr);
		read(nullptr);	
#elif defined( TCODE_TARGET_LINUX ) 
		_pipeline.fire_filter_on_open(addr);
		if ( _write_buffers.empty() ){
			if ( _loop.dispatcher().bind( handle()
				, EPOLLIN
				, this )){
			} else {
				close( tcode::diagnostics::platform_error() );
			}			
		}
#endif
	} else {
		_loop.execute([this,addr]{
			fire_on_open(addr);
		});
	}
}

void channel::fire_on_end_reference( void ){
	_pipeline.fire_filter_on_end_reference();
	delete this;
}

void channel::do_write( tcode::buffer::byte_buffer buf ){
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG ))
		return;

	if ( !_loop.in_event_loop() ){
		add_ref();
		_loop.execute([this,buf]{
			do_write( buf );
			release();
		});
		return;
	}
	bool write = _write_buffers.empty();
	_write_buffers.push_back( buf );
	if ( write ) {
#if defined( TCODE_TARGET_WINDOWS )
		write_remains( nullptr );
#elif defined( TCODE_TARGET_LINUX ) 
		if ( _loop.dispatcher().bind( handle()
			, EPOLLIN | EPOLLOUT
			, this)){
		} else {
			close( tcode::diagnostics::platform_error() );
		}
#endif
	}
}

void channel::do_write( tcode::buffer::byte_buffer buf1 
				, tcode::buffer::byte_buffer buf2 )
{
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG ))
		return;

	if ( !_loop.in_event_loop() ){
		add_ref();
		_loop.execute([this,buf1,buf2]{
			do_write( buf1,buf2 );
			release();
		});
		return;
	}
	bool write = _write_buffers.empty();
	_write_buffers.push_back( buf1 );
	_write_buffers.push_back( buf2 );
	if ( write ) {
#if defined( TCODE_TARGET_WINDOWS )
		write_remains( nullptr );
#elif defined( TCODE_TARGET_LINUX ) 
		if ( _loop.dispatcher().bind( handle()
			, EPOLLIN | EPOLLOUT
			, this)){
		} else {
			close( tcode::diagnostics::platform_error() );
		}
#endif
	}
}



#if defined( TCODE_TARGET_WINDOWS )
void channel::read( completion_handler_read* h ) {
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		if ( h )  delete h;
		return;
	}
		
	if ( h == nullptr ) h = new completion_handler_read( *this );
		
	h->prepare();
	tcode::iovec iov = h->read_buffer( channel_config::READ_BUFFER_SIZE );
	DWORD flag = 0;
    if ( WSARecv(	handle() 
                    , &iov
                    , 1
                    , nullptr 
                    , &flag 
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

void channel::handle_read( const tcode::diagnostics::error_code& ec 
		, tcode::buffer::byte_buffer& readbuf
		, completion_handler_read* h )
{
	if ( tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		if ( ec ){
			close(ec);
		} else {
			_pipeline.fire_filter_on_read( readbuf );
			read(h);
			return;
		}
	}
	delete h;
}

void channel::handle_write( const tcode::diagnostics::error_code& ec 
		, const int completion_bytes 
		, completion_handler_write* h )
{	
	if ( tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		if ( ec ) {
			close(ec);
		} else {
			int bytes = completion_bytes;
			auto it = _write_buffers.begin();
			while ( bytes > 0 ) {
				int move = it->rd_ptr( bytes );
				bytes -= move;
				++it;
			}
			_write_buffers.erase( 
				std::remove_if( _write_buffers.begin() , _write_buffers.end() , 
						[] ( tcode::buffer::byte_buffer& buf ) -> bool {
							return buf.length() == 0;
						}) , _write_buffers.end());

			bool flush = _write_buffers.empty();

			_pipeline.fire_filter_on_write( completion_bytes , flush );
			if ( !flush ) {
				write_remains( h );
				return;				
			}	
		}
	}	
	delete h;
}

void channel::write_remains( completion_handler_write* h ){
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		if ( h )  delete h;
		return;
	}
	if ( h == nullptr ) h = new completion_handler_write( *this );

	iovec write_buffers[16];
	int count = 0 ;
	auto it = _write_buffers.begin();
	for ( ; count < 16 ; ++count , ++it) {
		if ( it == _write_buffers.end() ){
			break;
		}
		write_buffers[count] = tcode::io::write_buffer( *it );
	}
	h->prepare();
	DWORD flag = 0;
    if ( WSASend( handle() 
				, write_buffers
				, count
				, nullptr 
				, flag 
				, h
				, nullptr ) == SOCKET_ERROR )
	{
		std::error_code ec = tcode::diagnostics::platform_error();
        if ( ec.value() != WSA_IO_PENDING ){
			h->error_code(ec);
			_loop.execute_handler(h);
        }
	}
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
		if ( events & EPOLLOUT ){
			handle_write();
		}
	}
}

int channel::read( iovec* iov , int cnt ) {
	int result = -1;
	do{
		result = readv( handle() , iov , cnt );
	} while( result < 0 &&  errno == EINTR  );
	return result;
}

int channel::write( iovec* iov , int cnt ){
	int result = -1;
	do{
		result = writev( handle() , iov , cnt );
	} while( result < 0 &&  errno == EINTR  );
	return result;
}

void channel::write_reamins( void ){
	iovec write_buffers[16];
	int count = 0 ;
	auto it = _write_buffers.begin();
	for ( ; count < 16 ; ++count , ++it) {
		if ( it == _write_buffers.end() ){
			break;
		}
		write_buffers[count] = tcode::io::write_buffer( *it );
	}
	int result = write( write_buffers , count );
	if ( result < 0 ){
		close( tcode::diagnostics::platform_error() );
	} else if ( result == 0 ) {
		close( std::error_code( ECONNRESET , tcode::diagnostics::posix_category()) );
	} else {
		int wr_bytes = result;
		it = _write_buffers.begin();
		while ( result > 0 ) {
			int move = it->rd_ptr( result );
			result -= move;
			++it;
		}
		_write_buffers.erase(
			std::remove_if( _write_buffers.begin() , _write_buffers.end() ,
					[] ( tcode::buffer::byte_buffer& buf ) -> bool {
						return buf.length() == 0;
					}) , _write_buffers.end());
		bool flush = _write_buffers.empty();
		_pipeline.fire_filter_on_write( wr_bytes , flush );
	}
}

void channel::handle_read( void ){
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		return;
	}

	tcode::buffer::byte_buffer buffer( channel_config::READ_BUFFER_SIZE );	
	iovec vec = tcode::io::read_buffer( buffer );
	int result = read( &vec , 1 );
	if ( result < 0 ){
		close( tcode::diagnostics::platform_error() );
	} else if ( result == 0 ) {
		close( std::error_code( ECONNRESET , tcode::diagnostics::posix_category()) );
	} else {
		buffer.wr_ptr( result );
		_pipeline.fire_filter_on_read( buffer );
	}	
}

void channel::handle_write( void ){
	if ( !tcode::threading::atomic_bit_on( _flag , detail::NOT_CLOSED_FLAG )) {
		return;
	}
	write_reamins();
	if ( _write_buffers.empty()) {
		if ( _loop.dispatcher().bind( handle()
			, EPOLLIN
			, this )){
		} else {
			close( tcode::diagnostics::platform_error() );
		}		
	}
}
#endif

}}}