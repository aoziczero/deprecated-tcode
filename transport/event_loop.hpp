#ifndef __tcode_transport_event_loop_h__
#define __tcode_transport_event_loop_h__

#if defined( TCODE_TARGET_WINDOWS )
#include <io/completion_port.hpp>
#elif defined( TCODE_TARGET_LINUX )
#include <io/epoll.hpp>
#endif
#include <io/completion_handler_one_shot.hpp>
#include <transport/event_timer.hpp>

#include <thread>
#include <list>


namespace tcode { namespace transport {

#if defined( TCODE_TARGET_WINDOWS )
typedef	tcode::io::completion_port dispatcher;
#elif defined( TCODE_TARGET_LINUX )
typedef	tcode::io::epoll dispatcher;
#endif

class event_loop {
public:
	event_loop( void );
	~event_loop( void );

	void links_add( void );
	void links_release( void );

	transport::dispatcher& dispatcher( void );

	void run( void );

	bool in_event_loop( void );

	void execute( tcode::io::completion_handler* handler );
	
	template < typename Handler >
	void execute( const Handler& handler ) {
		tcode::io::completion_handler* h =
			new tcode::io::completion_handler_one_shot_no_param< Handler >( handler );
		execute(h);
	}

	void schedule( const event_timer::pointer_type& ptr );
	void cancel( const event_timer::pointer_type& ptr );
	tcode::time_span wake_up_time( void );
	void schedule_timer( void );
private:
	transport::dispatcher _dispatcher;
	std::atomic< int > _active_links;
	std::thread::id _thread_id;
	std::list< event_timer::pointer_type > _event_timers;
};

}}


#endif