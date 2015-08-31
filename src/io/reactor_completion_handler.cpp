#include "stdafx.h"
#include <tcode/io/reactor_completion_handler.hpp>

namespace tcode { namespace io {

    reactor_completion_handler::reactor_completion_handler(
            base_type::completion_function fn , io_function iofn ) 
        : base_type( fn ) , _io_function( iofn )
    {
    }

    reactor_completion_handler::~reactor_completion_handler( void ){
    }

    bool reactor_completion_handler::do_reactor_io( void* handle ) {
        return _io_function( this , handle );
    }

}}
