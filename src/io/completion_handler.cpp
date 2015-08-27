#include "stdafx.h"
#include <tcode/io/completion_handler.hpp>

namespace tcode { namespace io {

    completion_handler::completion_handler( 
            completion_handler::call_function fn )
        : _call_function( fn ) 
         ,_next(nullptr)
    {
    }

    completion_handler::~completion_handler( void ) {
    }

    void completion_handler::operator()( void* handle , int ev ) {
        //cassert( _call_function != nullptr );
        _call_function( handle , ev , this );
    }

    completion_handler*& completion_handler::next( void ) {
        return _next;
    }

}}
