#include "stdafx.h"
#include <tcode/io/ip/tcp/operation_connect.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    operation_connect_base::operation_connect_base(
            tcode::operation::execute_handler fn 
            , const tcode::io::ip::address& addr )
        : tcode::io::operation( fn , &operation_connect_base::post_connect )
        , _address( addr )
    {
    }

    operation_connect_base::~operation_connect_base(void){
    }

    bool operation_connect_base::post_connect( io::operation* op
            , io::multiplexer* mux 
            , io::descriptor desc )
    {
        return true;
    }

    tcode::io::ip::address& operation_connect_base::address( void ) {
        return _address;
    }


}}}}
