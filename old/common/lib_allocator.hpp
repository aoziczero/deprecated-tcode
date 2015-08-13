#ifndef __tcode_common_lib_allocator_h__
#define __tcode_common_lib_allocator_h__

namespace tcode {

void* alloc( const std::size_t sz );
void  free( void* ptr );
void* realloc( void* ptr , const std::size_t sz );
		
typedef void* (*fn_malloc)( const std::size_t sz);
typedef void  (*fn_free)( void* ptr  );
typedef void* (*fn_realloc)( void* ptr , const std::size_t sz);
 
fn_malloc  hook_alloc( fn_malloc fn );
fn_free    hook_free( fn_free fn );
fn_realloc hook_realloc( fn_realloc fn);



template < typename Object >
class object_allocator {
public:
	//! construct
	object_allocator( void ) {
	}

	//! destruct
	~object_allocator( void ) {	
	}
#if defined( TCODE_TARGET_WINDOWS ) 
	static void* operator new( std::size_t sz ) {
#else
	static void* operator new( std::size_t sz ) throw ( std::bad_alloc ){
#endif
		return tcode::alloc( sz );
		//return ::operator new ( sz );
	}

	static void operator delete  ( void* ptr ) {
		return tcode::free(ptr);
	}

	static void operator delete ( void* ptr, std::size_t size ) throw () {
		return tcode::free(ptr);
	}
	

	static void* operator new ( std::size_t sz
							, const std::nothrow_t & ) throw (){
		return tcode::alloc( sz );
	}

	static void operator delete ( void* ptr, const std::nothrow_t & ) throw(){
		return tcode::free(ptr);
	}


	static void* operator new( std::size_t sz , void* place ){
		return place;	
	}
	
	static void operator delete ( void* p, void* place ) {
		//
	}
};

}

#endif