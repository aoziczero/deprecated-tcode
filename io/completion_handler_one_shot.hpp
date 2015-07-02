#ifndef __tcode_io_completion_handler_t_h__
#define __tcode_io_completion_handler_t_h__

#include <io/completion_handler.hpp>

namespace tcode { namespace io {

template < typename Handler >
class completion_handler_one_shot
	: public tcode::io::completion_handler
{
public:
	completion_handler_one_shot( const Handler& h )
		: _handler(h)
	{
	}
	virtual ~completion_handler_one_shot( void ){
	}
	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes ) 
	{
		_handler( ec ,  completion_bytes );
		delete this;
	}
private:	
	Handler _handler;
};


template < typename Handler >
class completion_handler_one_shot_no_param
	: public tcode::io::completion_handler
{
public:
	completion_handler_one_shot_no_param( const Handler& h )
		: _handler(h)
	{
	}
	virtual ~completion_handler_one_shot_no_param( void ){
	}
	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes ) 
	{
		_handler();
		delete this;
	}
private:	
	Handler _handler;
};



}}

#endif