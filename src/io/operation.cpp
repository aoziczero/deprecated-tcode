#include "stdafx.h"
#include <tcode/io/operation.hpp>

namespace tcode { namespace io {

    operation::operation( tcode::operation::execute_handler exh
            , post_proc_handler pph )
        : tcode::operation( exh )
          , _post_proc( pph )
    {
    }

    operation::~operation( void ) {

    }

    bool operation::post_proc( void* desc ){
        return _post_proc( this , desc );
    }

    std::error_code& operation::error( void ) {
        return _error;
    }

}}
