#include "stdafx.h"
#include <tcode/io/handler.hpp>


namespace tcode { namespace io {

    handler::handler( void ) 
        : _callback(nullptr) , _userctx(nullptr)
    {
    
    }

    handler::handler( callback cb , void* userctx )
        : _callback(cb) , _userctx(userctx)
    {
        
    }

    handler::~handler( void ) {
    
    }

    void handler::operator()( void* ioctx ){
        if ( _callback )
            _callback( _userctx , ioctx );
    }

    void handler::bind( callback cb , void* userctx ){
        _callback = cb;
        _userctx = userctx;
    }
}}

