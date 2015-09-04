#include "stdafx.h"
#include <tcode/io/ip/tcp/operation_accept.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
namespace tcode { namespace io { namespace ip { namespace tcp {

    operation_accept_base::operation_accept_base(
            tcode::operation::execute_handler fn 
            , tcode::io::ip::tcp::socket& fd )
        : tcode::io::operation( fn , &operation_accept_base::post_accept )
        , _fd( fd )
    {
    }

    operation_accept_base::~operation_accept_base(void){
    }
    
    bool operation_accept_base::post_accept_impl( io::multiplexer* mux 
                , io::descriptor desc )
    {
        int r = mux->accept( desc 
                    , _fd.descriptor() 
                    , _address
#if defined( TCODE_WIN32 )
					, _accepted_fd
					, _address_buf
#endif
                    , error());

        if (error() || r >= 0 )
            return true;
        return false;
    }

    bool operation_accept_base::post_accept( io::operation* op_base
            , io::multiplexer* mux 
            , io::descriptor desc )
    {
        operation_accept_base* op( 
                static_cast< operation_accept_base* >(
                    op_base ));
        return op->post_accept_impl( mux , desc );
    }

    tcode::io::ip::address& operation_accept_base::address( void ) {
		return _address;
    }


}}}}
