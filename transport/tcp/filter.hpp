#ifndef __tcode_transport_tcp_filter_h__
#define __tcode_transport_tcp_filter_h__

#include <buffer/byte_buffer.hpp>
#include <io/ip/address.hpp>
#include <diagnostics/tcode_error_code.hpp>

namespace tcode { namespace transport { namespace tcp {

class channel;
class pipeline;
class filter {
public:
	filter( void );
	virtual ~filter( void );
	
	// inbound
	virtual void filter_on_open( const tcode::io::ip::address& addr );
	virtual void filter_on_close( void );
	virtual void filter_on_read( tcode::buffer::byte_buffer buf );
	virtual void filter_on_write( int written , bool flush );
	virtual void filter_on_error( const std::error_code& ec );

	// filter_on_end_reference 는 지연 실행 불가
	virtual void filter_on_end_reference( void );;
	
	// outbound
	virtual void filter_do_write( tcode::buffer::byte_buffer buf );	
	//virtual void filter_do_writev( const std::vector< tcode::buffer::byte_buffer>& bufs );	

	// fire inbound
	void fire_filter_on_open( const tcode::io::ip::address& addr );
	void fire_filter_on_close( void );
	void fire_filter_on_read( tcode::buffer::byte_buffer& buf );
	void fire_filter_on_write( int written , bool flush );
	void fire_filter_on_error( const std::error_code& ec );
	void fire_filter_on_end_reference( void );;
	
	void fire_filter_do_write( tcode::buffer::byte_buffer& buf );
	//void fire_filter_do_writev( const std::vector< tcode::buffer::byte_buffer>& bufs );	

	filter* inbound( void );
	filter* outbound( void );

	void inbound( filter* f );
	void outbound( filter* f );
	
	tcp::channel* channel( void );
	tcp::pipeline* pipeline( void );
	void pipeline( tcp::pipeline* p );

	void add_ref( void );
	void release( void );

	void close( const tcode::diagnostics::error_code& ec );
private:
	tcp::pipeline* _pipeline;
	filter* _inbound;
	filter* _outbound;
};

}}}

#endif