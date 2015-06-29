#ifndef __tcode_object_factory_h__
#define __tcode_object_factory_h__

#include <common/closure.hpp>

namespace tcode {

template < typename Object , typename Key>
class object_factory {
public:
	typedef handler< Object* ( ) > create_handler;
	object_factory( void ) {
	}

	~object_factory( void ) {
	}
	
	template < typename Handler >
	void register_handler( const Key& key , const Handler& handler ) {
		_handler_map.insert( std::map< Key , create_handler >::value_type( key , handler ));
	}
	
	Object* create_object( const Key& key ) {
		std::map< Key , create_handler >::iterator it = _handler_map.find( key );
		if ( it != _handler_map.end() ) {
			return (it->second)();
		}
		return nullptr;
	}
private:
	std::map< Key , create_handler > _handler_map;
};

}

#endif