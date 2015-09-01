#include "stdafx.h"
#include <tcode/operation.hpp>

namespace tcode {
namespace detail{
    allocator op_allocator;
}

    operation::operation( operation::execute_handler fn )
        : _execute( fn )
    {
    }

    operation::~operation( void ) {
    }

    void operation::operator()( void ) {
        _execute( this );
    }

    void* operation::alloc( int sz ){
        return detail::op_allocator.alloc(sz);
    }

    void operation::free( void* p ){
        detail::op_allocator.free(p);
    }
    
    tcode::allocator& operation::allocator( void ) {
        return detail::op_allocator;
    }
}
