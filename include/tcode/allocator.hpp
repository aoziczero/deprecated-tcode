#ifndef __tcode_allocator_h__
#define __tcode_allocator_h__

namespace tcode {

    struct allocator{
        void* (*alloc)( const std::size_t );
        void  (*free)( void* );

        static allocator malloc_allocator( void );
    };

}

#endif
