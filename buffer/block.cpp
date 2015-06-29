#include "stdafx.h"
#include "block.hpp"
#include "fixed_size_allocator.hpp"
#include <common/lib_allocator.hpp>
#include <threading/spin_lock.hpp>
#include <atomic>


namespace tcode { namespace buffer { 
namespace block {

namespace impl {

static const std::size_t ALLOC_SIZE_TABLE[]			= { 4 , 8 , 16 , 32 , 64 , 256 , 1024 , 2048 , 4096 , 8192 , 16384 };
static const std::size_t ALLOC_SIZE_TABLE_COUNT		= 11;

struct block_hdr {	
	std::atomic<int> refcount;
	std::size_t capacity;
};


class allocator {
public:
	allocator( void ){
		for ( int i = 0 ; i < ALLOC_SIZE_TABLE_COUNT ; ++i ) {
			_fixed_size_allocators[i].init( ALLOC_SIZE_TABLE[i]  + sizeof( block_hdr ) );
		}
	}

	~allocator( void ) {

	}
	
	handle	alloc( const std::size_t size ) {
		block_hdr* pb = nullptr;
		for ( int i = 0 ; i < ALLOC_SIZE_TABLE_COUNT ; ++i ) {
			if ( ALLOC_SIZE_TABLE[i] >= size ) {
				pb = reinterpret_cast< block_hdr* >( _fixed_size_allocators[i].alloc());
				pb->capacity = ALLOC_SIZE_TABLE[i];
				break;
			}
		}
		if ( pb == nullptr ) {
			pb = reinterpret_cast< block_hdr* >(  tcode::alloc( size + sizeof( block_hdr ) ) );
			pb->capacity = size;
		}
		pb->refcount.store(1);
		return reinterpret_cast<handle>(pb + 1);
	}

	handle	realloc( handle ptr , const std::size_t size ) {
		if ( ptr == nullptr )
			return alloc( size );
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
			
		if( pb->capacity >= size ) 
			return ptr;
		
		handle nptr = alloc( size );
		memcpy( nptr , ptr , pb->capacity );
		
		free( ptr );
		return nptr;
	}

	void	free( handle ptr ){
		if ( ptr == nullptr )
			return ;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		if (pb->refcount.fetch_sub(1) == 1) {
			for ( int i = 0 ; i < ALLOC_SIZE_TABLE_COUNT ; ++i ) {
				if ( ALLOC_SIZE_TABLE[i] == pb->capacity ) {
					_fixed_size_allocators[i].free( pb );
					return;
				}
			}
			tcode::free( pb );
		}
	}

	std::size_t capacity( handle ptr ) {
		if ( ptr == nullptr )
			return 0;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		return pb->capacity;
	}

	int			refcount( handle ptr ) {
		if ( ptr == nullptr )
			return 0;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		return pb->refcount.load();
	}

	handle		duplicate( handle ptr ) {
		if ( ptr == nullptr )
			return nullptr;
		block_hdr* pb = reinterpret_cast< block_hdr* >(ptr) - 1;
		pb->refcount.fetch_add(1);
		return ptr;
	}

	handle      copy( handle ptr ){
		if ( ptr == nullptr )
			return nullptr;
		
		std::size_t sz = capacity( ptr );
		
		handle cpy = alloc( sz );
		memcpy( cpy , ptr ,sz );		
		return cpy;
	}
private:
	typedef fixed_size_allocator< tcode::threading::spin_lock > fixed_size_allocator_impl;
	fixed_size_allocator_impl _fixed_size_allocators[ALLOC_SIZE_TABLE_COUNT];
};

allocator _allocator;

}

handle		alloc( const std::size_t sz ){
	return impl::_allocator.alloc( sz );
}

void		free( handle ptr ) {
	return impl::_allocator.free(ptr);
}

handle      realloc( handle ptr , const std::size_t nsz ) {
	return impl::_allocator.realloc(ptr , nsz);
}

std::size_t capacity( handle ptr ) {
	return impl::_allocator.capacity(ptr);
}

int			refcount( handle ptr ) {
	return impl::_allocator.refcount(ptr);
}

handle		duplicate( handle ptr ) {
	return impl::_allocator.duplicate(ptr);
}

handle      copy( handle ptr ){
	return impl::_allocator.copy(ptr);		
}

}
}}
