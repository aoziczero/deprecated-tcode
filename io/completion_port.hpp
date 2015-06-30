//! \file iocp.hpp

#ifndef __tcode_io_completion_port_h__
#define __tcode_io_completion_port_h__

#include <common/basic_handle.hpp>
#include <common/time_span.hpp>

#include <threading/spin_lock.hpp>

#include <io/completion_handler.hpp>


namespace tcode { namespace io {
class completion_handler;
class completion_port
	: public tcode::basic_handle< HANDLE >
{
public:
	explicit completion_port( const int num_of_proc = 0 );
	~completion_port( void );

	int run( const tcode::time_span& ts );

	bool bind( HANDLE h );
	void unbind( HANDLE h);

	void wake_up();		
	
	void post( tcode::io::completion_handler* ch );
private:
	tcode::threading::spin_lock _lock;
	tcode::slist::queue< tcode::io::completion_handler > _posted;
};

}}

#endif
