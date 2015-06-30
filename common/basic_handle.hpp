#ifndef __tcode_basic_handle_h__
#define __tcode_basic_handle_h__

#include <atomic>

namespace tcode {

template < typename Handle >
class basic_handle {
public:
	basic_handle( void ){
	
	}
	Handle handle( void ) const {
		return _handle.load();
	}

	Handle handle( Handle handle ) {
		return _handle.exchange( handle );
	}
protected:
	~basic_handle( void ){
	}
private:
	std::atomic< Handle > _handle;
};

}

#endif