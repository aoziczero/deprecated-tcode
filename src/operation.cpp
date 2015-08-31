#include "stdafx.h"
#include <tcode/operation.hpp>

namespace tcode {

    operation::operation( operation::execute_impl fn )
        : _execute( fn )
    {
    }

    operation::~operation( void ) {
    }

    void operation::operator()( void ) {
        _execute( this );
    }

}
