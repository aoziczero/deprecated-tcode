#ifndef __tcode_block_h__
#define __tcode_block_h__

namespace tcode { 
    
    class block {
    public:
        typedef struct _handle{
        }* handle;

        static handle		alloc( const std::size_t sz );
        static void		free( handle b );
        static handle      realloc( handle b , const std::size_t nsz );

        static std::size_t capacity( handle b );

        static int			refcount( handle b );
        static handle		duplicate( handle b );
        static handle       copy( handle b );
    };
}

#endif
