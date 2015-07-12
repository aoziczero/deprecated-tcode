#ifndef __tcode_transport_tcp_channel_h__
#define __tcode_transport_tcp_channel_h__


#include <buffer/byte_buffer.hpp>
#include <io/ip/basic_socket.hpp>
#include <transport/tcp/pipeline.hpp>

#include <atomic>

#if defined( TCODE_TARGET_LINUX )
#include <io/epoll.hpp>
#endif

namespace tcode{ namespace transport { 

class event_loop;

namespace tcp{ 

#if defined( TCODE_TARGET_WINDOWS )
class completion_handler_read;
class completion_handler_write; 
#endif

class pipeline;
class channel 
	: public tcode::io::ip::tcp_base 
#if defined( TCODE_TARGET_LINUX )
	, public tcode::io::epoll::handler
#endif
{
public:
	explicit channel( event_loop& l 
					, const tcp::pipeline& p 
					, tcode::io::ip::socket_type fd );
#if defined( TCODE_TARGET_WINDOWS )
	~channel( void );
#else
	virtual ~channel( void );
#endif
	event_loop& loop( void );
	tcp::pipeline& pipeline( void );

	void close( void );
	void close( const std::error_code& ec );
	
	void add_ref( void );
	void release( void );
	
	void fire_on_open( const tcode::io::ip::address& addr );
	void fire_on_end_reference( void );
	void fire_on_close( const std::error_code& ec );

	void do_write( tcode::buffer::byte_buffer buf );
	void do_write( tcode::buffer::byte_buffer buf1 
				, tcode::buffer::byte_buffer buf2 );

#if defined( TCODE_TARGET_WINDOWS )
	void read( completion_handler_read* h );
	void handle_read( const tcode::diagnostics::error_code& ec 
			, tcode::buffer::byte_buffer& readbuf
			, completion_handler_read* h );
	void handle_write( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes 
			, completion_handler_write* h );
	void write_remains( completion_handler_write* h );
#elif defined( TCODE_TARGET_LINUX )
	virtual void operator()( const int events );
	int read( iovec* iov , int cnt );
	int write( iovec* iov , int cnt );
	void write_reamins( void );
	void handle_read( void );
	void handle_write( void );	
#endif	
private:
	event_loop& _loop;
	tcp::pipeline _pipeline;
	std::atomic< int > _flag;
	std::vector< tcode::buffer::byte_buffer > _write_buffers;
};

}}}


#endif
