#include "stdafx.h"
#include <tcode/active_ref.hpp>

namespace tcode {

    active_ref::active_ref( void ){
        _count.store(0);
    }

    active_ref::~active_ref( void ){

    }

    int active_ref::inc( void ) {
        return _count.fetch_add(1) + 1;
    }

    int active_ref::dec( void ) {
        return _count.fetch_sub(1) - 1;
    }

    int active_ref::count( void ) {
        return _count.load();
    }
}
