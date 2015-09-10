#include "stdafx.h"
#include <tcode/block.hpp>
#include <tcode/threading/spinlock.hpp>
#include <tcode/fixed_size_allocator.hpp>
#include <atomic>

namespace tcode { 
namespace {
    static const std::size_t alloc_size_table[] = {
        4 , 8 , 16 , 32 , 64 , 256 , 1024 , 2048 , 4096 , 8192 , 16384
    };
    static const std::size_t alloc_size_table_count		= 11;
}

class block_manager{
public:
    struct block_hdr {	
        std::atomic<int> refcount;
        std::size_t capacity;
    };

	block_manager( void ){
		for ( int i = 0 ; i < alloc_size_table_count ; ++i ) {
			_fixed_size_allocators[i].init( alloc_size_table[i]  + sizeof( block_hdr ) );
		}
	}

	~block_manager( void ) {

	}
	
	block::handle	alloc( const std::size_t size ) {
		block_hdr* pb = nullptr;
		for ( int i = 0 ; i < alloc_size_table_count ; ++i ) {
			if ( alloc_size_table[i] >= size ) {
				pb = reinterpret_cast< block_hdr* >( _fixed_size_allocators[i].alloc());
				pb->capacity = alloc_size_table[i];
				break;
			}
		}
		if ( pb == nullptr ) {
			pb = reinterpret_cast< block_hdr* >(  std::malloc( size + sizeof( block_hdr ) ) );
			pb->capacity = size;
		}
		pb->refcount.store(1);
		return reinterpret_cast<block::handle>(pb + 1);
	}

	block::handle	realloc( block::handle ptr , const std::size_t size ) {
		if ( ptr == nullptr )
			return alloc( size );
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
			
		if( pb->capacity >= size ) 
			return ptr;
		
		block::handle nptr = alloc( size );
		memcpy( nptr , ptr , pb->capacity );
		
		free( ptr );
		return nptr;
	}

	void	free( block::handle ptr ){
		if ( ptr == nullptr )
			return ;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		if (pb->refcount.fetch_sub(1) == 1) {
			for ( int i = 0 ; i < alloc_size_table_count ; ++i ) {
				if ( alloc_size_table[i] == pb->capacity ) {
					_fixed_size_allocators[i].free( pb );
					return;
				}
			}
			std::free( pb );
		}
	}

	std::size_t capacity( block::handle ptr ) {
		if ( ptr == nullptr )
			return 0;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		return pb->capacity;
	}

	int			refcount( block::handle ptr ) {
		if ( ptr == nullptr )
			return 0;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		return pb->refcount.load();
	}

	block::handle		duplicate( block::handle ptr ) {
		if ( ptr == nullptr )
			return nullptr;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		pb->refcount.fetch_add(1);
		return ptr;
	}

	block::handle      copy( block::handle ptr ){
		if ( ptr == nullptr )
			return nullptr;
		
		std::size_t sz = capacity( ptr );
		
		block::handle cpy = alloc( sz );
		memcpy( cpy , ptr ,sz );		
		return cpy;
	}

    static block_manager& instance() {
        static block_manager manager;
        return manager;
    }
private:
	typedef fixed_size_allocator< tcode::threading::spinlock > fixed_size_allocator_impl;
	fixed_size_allocator_impl _fixed_size_allocators[alloc_size_table_count];
};

block::handle		block::alloc( const std::size_t sz ){
	return block_manager::instance().alloc( sz );
}

void		block::free( block::handle ptr ) {
	return block_manager::instance().free(ptr);
}

block::handle      block::realloc( block::handle ptr , const std::size_t nsz ) {
	return block_manager::instance().realloc(ptr , nsz);
}

std::size_t block::capacity( block::handle ptr ) {
	return block_manager::instance().capacity(ptr);
}

int			block::refcount( block::handle ptr ) {
	return block_manager::instance().refcount(ptr);
}

block::handle		block::duplicate( block::handle ptr ) {
	return block_manager::instance().duplicate(ptr);
}

block::handle      block::copy( block::handle ptr ){
	return block_manager::instance().copy(ptr);		
}

}
