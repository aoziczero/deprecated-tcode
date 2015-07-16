#ifndef __tcode_transport_udp_channel_h__
#define __tcode_transport_udp_channel_h__


#include <buffer/byte_buffer.hpp>
#include <io/ip/basic_socket.hpp>
#include <transport/udp/pipeline.hpp>

#include <atomic>

#if defined( TCODE_TARGET_LINUX )
#include <io/epoll.hpp>
#endif

namespace tcode{ namespace transport { 

class event_loop;

namespace udp{ 

class channel_config {
public:
	static int READ_BUFFER_SIZE;
};


#if defined( TCODE_TARGET_WINDOWS )
class completion_handler_recv_from;
#endif

class pipeline;
class channel 
	: public tcode::io::ip::udp_base 
#if defined( TCODE_TARGET_LINUX )
	, public tcode::io::epoll::handler
#endif
{
public:
	explicit channel( event_loop& l 
					, const udp::pipeline& p 
					, tcode::io::ip::socket_type fd );
#if defined( TCODE_TARGET_WINDOWS )
	~channel( void );
#else
	virtual ~channel( void );
#endif
	event_loop& loop( void );
	udp::pipeline& pipeline( void );

	void close( void );
	void close( const std::error_code& ec );
	
	void add_ref( void );
	int release( void );
	
	void fire_on_open();
	void fire_on_end_reference( void );
	void fire_on_close( const std::error_code& ec );

	void do_write( const tcode::io::ip::address& addr 
		, tcode::buffer::byte_buffer buf );
#if defined( TCODE_TARGET_WINDOWS )
	void read( completion_handler_recv_from* h );
	void handle_recv_from( const tcode::diagnostics::error_code& ec 
			, tcode::buffer::byte_buffer& readbuf
			, const tcode::io::ip::address& addr 			
			, completion_handler_recv_from* h );
#elif defined( TCODE_TARGET_LINUX )
	int read( iovec* iov , int cnt , tcode::io::ip::address& addr );
	virtual void operator()( const int events );
	void handle_read( void );
#endif	
private:
	event_loop& _loop;
	udp::pipeline _pipeline;
	std::atomic< int > _flag;
};

}}}


#endif
