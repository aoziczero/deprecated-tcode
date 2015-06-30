#ifndef __tcode_transport_event_loop_h__
#define __tcode_transport_event_loop_h__

#if defined( TCODE_TARGET_WINDOWS )
#include <io/completion_port.hpp>
#elif defined( TCODE_TARGET_LINUX )
#include <io/epoll.hpp>
#endif

#include <thread>

namespace tcode { namespace transport {

class event_loop {
public:
	event_loop( void );
	~event_loop( void );


	void links_add( void );
	void links_release( void );
private:
#if defined( TCODE_TARGET_WINDOWS )
	tcode::io::completion_port dispatcher;
#elif defined( TCODE_TARGET_LINUX )
	tcode::io::epoll dispatcher;
#endif
	std::atomic< int > _active_links;
	std::thread::id _thread_id;
};

}}


#endif