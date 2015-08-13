#ifndef __tcode_buffer_block_h__
#define __tcode_buffer_block_h__

namespace tcode { namespace buffer { 
namespace block {

typedef struct _handle{
}* handle;

handle		alloc( const std::size_t sz );
void		free( handle b );
handle      realloc( handle b , const std::size_t nsz );

std::size_t capacity( handle b );

int			refcount( handle b );
handle		duplicate( handle b );
handle      copy( handle b );

}
}}

#endif