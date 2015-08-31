#include "stdafx.h"
#include <tcode/io/completion_handler.hpp>

namespace tcode { namespace io {

    void* completion_handler_alloc( int sz ){
        return operator new(sz);
    }

    void completion_handler_free( void* p ) {
        operator delete( p );
    }

}}
