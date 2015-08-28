#include "stdafx.h"
#include <tcode/io/reactor_completion_handler.hpp>

namespace tcode { namespace io {

    reactor_completion_handler::reactor_completion_handler(
            call_function fn 
          , io_function iofn ) 
        : completion_handler( fn ) 
         , _io_function( iofn )
    {
    }

    reactor_completion_handler::~reactor_completion_handler( void ){
    }

    bool reactor_completion_handler::operator()( void ) {
        return _io_function( this );
    }

}}
