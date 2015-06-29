#ifndef __tcode_threading_wrapped_handler_h__
#define __tcode_threading_wrapped_handler_h__

#include <threading/tls_data.hpp>

namespace tcode { namespace threading {

template < typename Handler >
class wrapped_handler {
public:
	wrapped_handler( const Handler& handler )
		: _handler( handler )
	{
	}
	
	~wrapped_handler( void ) {
	}

	wrapped_handler( wrapped_handler&& h )
		: _handler( std::move( h._handler )) 
	{
	}
		
	wrapped_handler( const wrapped_handler& rhs )
		: _handler( h._handler )
	{
	}

	void operator()( void ) {
		tls_data td;
		tls_data::instance(&td);
		_handler();
		tls_data::instance(nullptr);
	}
private:
	Handler _handler;
};

template < typename Handler >
wrapped_handler<Handler> bind( const Handler& h ) {
	return wrapped_handler<Handler>(h);
}

}}

#endif