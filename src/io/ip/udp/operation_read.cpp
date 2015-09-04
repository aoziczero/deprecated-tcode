#include "stdafx.h"
#include <tcode/io/ip/udp/operation_read.hpp>

namespace tcode { namespace io { namespace ip { namespace udp {

    operation_read_base::operation_read_base( 
            tcode::operation::execute_handler fn
            , const tcode::io::buffer& buffer)
        : tcode::io::operation( fn , &operation_read_base::post_read )
        , _buffer( buffer ) , _read(0) 
    {
    }

    operation_read_base::~operation_read_base( void ){
    }

    bool operation_read_base::post_read_impl( io::multiplexer* mux 
            , io::descriptor desc )
    {
#if defined( TCODE_WIN32 )
		_read = io_byte();
		return true;
#else
        _read = mux->read( desc , _buffer , _address , error() );
        if ( error() || _read >= 0 ) 
            return true;
        return false;
#endif
    }

    int operation_read_base::read_size( void ) {
        return _read;
    }

    tcode::io::ip::address& operation_read_base::address( void ){
        return _address; 
    }

	tcode::io::buffer& operation_read_base::buffer(void) {
		return _buffer;
	}

    bool operation_read_base::post_read( 
            io::operation* op_base 
            , io::multiplexer* mux 
            , io::descriptor desc )
    {
        operation_read_base* op( static_cast< operation_read_base* >(op_base));
        return op->post_read_impl( mux ,desc ); 
    }


}}}}
