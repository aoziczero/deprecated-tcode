#ifndef __tcode_allocator_h__
#define __tcode_allocator_h__

namespace tcode {

    struct allocator{
        allocator( void );

        void* (*alloc)( const std::size_t );
        void  (*free)( void* );
    };

}

#endif
