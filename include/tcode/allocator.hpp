#ifndef __tcode_allocator_h__
#define __tcode_allocator_h__

namespace tcode {

	/*!
		@class	allocator
		@brief	할당자 인터페이스
		@detail	변경을 최소화 하기 위해 custom allocator 가 필요한 object 등은
		우선 이 interface 를 사용하고 차후에 매핑하여 사용할 용도
		현재는 std::xxx 기본함수로 매핑처리되어 잇음
	*/
    struct allocator{
        allocator( void );

        void* (*alloc)( const std::size_t );
		void* (*realloc)(void* p, const std::size_t sz);
        void  (*free)( void* );
    };

}

#endif
