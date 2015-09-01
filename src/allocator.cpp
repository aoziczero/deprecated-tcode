#include "stdafx.h"
#include <tcode/allocator.hpp>

namespace tcode {

    allocator allocator::malloc_allocator(){
        allocator a;
        a.alloc = std::malloc;
        a.free = std::free;
        return a;
    }

}
