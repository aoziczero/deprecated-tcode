#ifndef __tcode_transport_tcp_channel_h__
#define __tcode_transport_tcp_channel_h__

#include <atomic>
#include <io/ip/basic_socket.hpp>
#include <transport/tcp/pipeline.hpp>

namespace tcode{ namespace transport { 

class event_loop;

namespace tcp{ 

class pipeline;
class channel 
	: public tcode::io::ip::tcp_base {
public:
	explicit channel( event_loop& l 
					, const tcp::pipeline& p 
					, tcode::io::ip::socket_type fd );
	~channel( void );

	event_loop& loop( void );
	tcp::pipeline& pipeline( void );

	void add_ref( void );
	void release( void );
	
	void fire_on_open( const tcode::io::ip::address& addr );
	void fire_on_end_reference( void );
private:
	event_loop& _loop;
	tcp::pipeline _pipeline;
	std::atomic< int > _flag;
};

}}}


#endif
