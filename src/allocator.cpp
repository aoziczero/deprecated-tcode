#include "stdafx.h"
#include <tcode/allocator.hpp>

namespace tcode {

    allocator::allocator( void ) {
        alloc = std::malloc;
        free = std::free;
		realloc = std::realloc;
    }

}
