#include "stdafx.h"
#include "lib_allocator.hpp"

namespace tcode {

lib_allocator::fn_malloc lib_malloc = &std::malloc;
lib_allocator::fn_free lib_free = &std::free;
lib_allocator::fn_realloc lib_realloc = &std::realloc;


void* lib_allocator::alloc( const std::size_t sz ) {
	return lib_malloc( sz );	
}

void  lib_allocator::free( void* ptr ){
	return lib_free( ptr );
}

void* lib_allocator::realloc( void* ptr , const std::size_t sz ){
	return lib_realloc( ptr , sz );
}

lib_allocator::fn_malloc  lib_allocator::hook_alloc( fn_malloc fn ){
	std::swap( lib_malloc , fn );
	return fn;
}

lib_allocator::fn_free    lib_allocator::hook_free( fn_free fn ){
	std::swap( lib_free , fn );
	return fn;
}

lib_allocator::fn_realloc lib_allocator::hook_realloc( fn_realloc fn){
	std::swap( lib_realloc , fn );
	return fn;
}

}