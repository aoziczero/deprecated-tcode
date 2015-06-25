#ifndef __tcode_common_lib_allocator_h__
#define __tcode_common_lib_allocator_h__

namespace tcode {

// 라이브러리 내부에서 사용할 할당함수 
// 기본 std::malloc 사용
// 성능이슈시 tcmalloc 으로 교체할것
class lib_allocator {
public:
	static void* alloc( const std::size_t sz );
	static void  free( void* ptr );
	static void* realloc( void* ptr , const std::size_t sz );
		
	typedef void* (*fn_malloc)( const std::size_t sz);
	typedef void  (*fn_free)( void* ptr  );
	typedef void* (*fn_realloc)( void* ptr , const std::size_t sz);
 
	static fn_malloc  hook_alloc( fn_malloc fn );
	static fn_free    hook_free( fn_free fn );
	static fn_realloc hook_realloc( fn_realloc fn);

	DISALLOW_CONSTRUCTOR_AND_COPY( lib_allocator );
};

}

#endif