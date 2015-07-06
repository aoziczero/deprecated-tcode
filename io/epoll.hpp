#ifndef __tcode_io_epoll_h__
#define __tcode_io_epoll_h__

#include <common/basic_handle.hpp>
#include <common/time_span.hpp>

#include <threading/spin_lock.hpp>

#include <io/completion_handler.hpp>

#include <sys/epoll.h>

namespace tcode { namespace io {

class epoll
	: public tcode::basic_handle< int >
{
public:
	class handler {
	public:
		handler( void );
		virtual ~handler( void );
		virtual void operator()( const int events ) = 0;
	};
public:
	epoll( void );
	~epoll( void );

	int run( const tcode::time_span& ts );

	bool bind( int fd , const int events , handler* phandler );
	void unbind( int fd );

	void wake_up();

	void post( tcode::io::completion_handler* ch );
	void drain_req( void );
private:
	class post_handler
		: public handler
	{
	public:
		post_handler( epoll& e );
		virtual ~post_handler( void );
		virtual void operator()( const int events );

		int handle( void );
		void set( void );
		void post( tcode::io::completion_handler* ch );
		void drain( void );
	private:
		int _event_fd;
		epoll& _epoll;
		tcode::threading::spin_lock _lock;
		tcode::slist::queue< tcode::io::completion_handler > _posted;
	};
	post_handler _post;
	bool _drain_req;
};

}}
#endif
