#include "stdafx.h"
#include "lib_allocator.hpp"

namespace tcode {

fn_malloc lib_malloc = &std::malloc;
fn_free lib_free = &std::free;
fn_realloc lib_realloc = &std::realloc;


void* alloc( const std::size_t sz ) {
	return lib_malloc( sz );	
}

void  free( void* ptr ){
	return lib_free( ptr );
}

void* realloc( void* ptr , const std::size_t sz ){
	return lib_realloc( ptr , sz );
}

fn_malloc  hook_alloc( fn_malloc fn ){
	std::swap( lib_malloc , fn );
	return fn;
}

fn_free    hook_free( fn_free fn ){
	std::swap( lib_free , fn );
	return fn;
}

fn_realloc hook_realloc( fn_realloc fn){
	std::swap( lib_realloc , fn );
	return fn;
}

}