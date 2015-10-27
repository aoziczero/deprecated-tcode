#ifndef __tcode_threading_local_storages_h__
#define __tcode_threading_local_storages_h__

#include <tcode/tcode.hpp>

namespace tcode { namespace threading {

	/*!
	*	@class local_storage
	*	@brief thread local storage wrapper
	*	@author tk
	*	@detail	win32 / pthread ¸¸ ±¸Çö
	*/
    class local_storage {
    public:
        local_storage( void );
        ~local_storage( void );

        void* value( void );
        void value( void* p );
    private:
#if defined( TCODE_WIN32 )
        DWORD _key;
#else
        pthread_key_t _key;
#endif
    };

	/*!
	*	@class local_storage_T
	*	@brief thread local storage template wrapper
	*	@author tk
	*/
    template < typename Object >
    class local_storage_T {
    public:
        local_storage_T( void ){
        }

        ~local_storage_T( void ) {
        }

        Object& value( void ) {
            Object* obj = reinterpret_cast<Object*>(local_storage_T::_storage.value());
            if ( obj == nullptr ) {
                obj = new Object();
                local_storage_T::_storage.value( obj );
            }
            return *obj;
        }
    private:
        static local_storage _storage;
    };

    template < typename Object >
    local_storage local_storage_T< Object >::_storage;
}}


#endif
