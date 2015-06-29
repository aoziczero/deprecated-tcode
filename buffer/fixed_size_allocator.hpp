#ifndef __tcode_fixed_size_allocator_h__
#define __tcode_fixed_size_allocator_h__
#include <vector>
#include <algorithm>

namespace tcode {
namespace {
    static const std::size_t FREE_THRESHOLD = 4096;
	static const std::size_t FREE_REMAINS   = static_cast<std::size_t>(FREE_THRESHOLD * 0.8);
}

template < typename Lock >
class fixed_size_allocator{
public:
    fixed_size_allocator( void ) 
		: _alloc_size( 0 ) {
	}

    ~fixed_size_allocator( void ){
        clean_up(); 
    }

    void  init( const std::size_t size ) {
        assert( size > 0 && "fixed_size_allocator::Init Param Size <= 0");
        clean_up();
        _alloc_size	 = size < sizeof( void* ) ? sizeof( void* ) : size;
        _pool.reserve( FREE_THRESHOLD );
    }

    void  clean_up( void ){
		tcode::threading::scoped_lock< Lock > guard(_lock );
        while ( !_pool.empty() ) {
            std::free( _pool.back() );
            _pool.pop_back();
        }            
    }

    void* alloc( void ) {
        void* ptr = nullptr;
        if ( !_pool.empty() ) {
            tcode::threading::scoped_lock< Lock > guard(_lock );
            if ( !_pool.empty() ) {
                ptr = _pool.back();
                _pool.pop_back();
				return ptr;
            }
        }
        return std::malloc( _alloc_size );
    }

    void free( void* ptr ) {
        if ( ptr == nullptr ) {
            return ;
        }
        tcode::threading::scoped_lock< Lock > guard(_lock );
#ifdef _DEBUG 
        if ( _pool.end() != std::find( _pool.begin() , _pool.end() , ptr ) ) {
            assert( !"Double Free" );
        }
#endif
		if ( _pool.size() > FREE_THRESHOLD ) {
			std::free( ptr );
			while( _pool.size() < FREE_REMAINS ) {
				ptr = _pool.back();
				_pool.pop_back();
				std::free( ptr );
			}
		} else {
			_pool.push_back( ptr );
		}
    }
	
private:
    Lock _lock;
	std::size_t _alloc_size;
	std::vector< void* > _pool;
};

}



#endif