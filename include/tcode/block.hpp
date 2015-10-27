#ifndef __tcode_block_h__
#define __tcode_block_h__

namespace tcode { 
    
	/*!
		@class	block
		@brief	refrenceCounting buffer interface
	*/
    class block {
    public:
		//! buffer handle
        typedef struct _handle{
        }* handle;

		static handle		alloc( const std::size_t sz );
        static void		free( handle b );
        static handle      realloc( handle b , const std::size_t nsz );

        static std::size_t capacity( handle b );

        static int			refcount( handle b );
		//! shallow copy
        static handle		duplicate( handle b );
		//! deep copy
        static handle       copy( handle b );
    };
}

#endif
