#include "stdafx.h"
#include <tcode/io/operation.hpp>
#include <tcode/error.hpp>
namespace tcode { namespace io {

    operation::operation( tcode::operation::execute_handler exh
            , post_proc_handler pph )
        : tcode::operation( exh )
        , _post_proc( pph )
		, _error( tcode::error_success )
    {
    }

    operation::~operation( void ) {

    }

    bool operation::post_proc( io::multiplexer* mux ,  io::descriptor desc ){
        return _post_proc( this , mux , desc );
    }

    std::error_code& operation::error( void ) {
        return _error;
    }

}}
