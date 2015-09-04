#include "stdafx.h"
#include <tcode/io/ip/udp/operation_write.hpp>

namespace tcode { namespace io { namespace ip { namespace udp {

    operation_write_base::operation_write_base( 
            tcode::operation::execute_handler fn
            , const tcode::io::buffer& buffer
            , const tcode::io::ip::address& addr)
        : tcode::io::operation( fn , &operation_write_base::post_write )
        , _address( addr ) , _buffer( buffer ) , _write(0) 
    {
    }

    operation_write_base::~operation_write_base( void ){
    }

    bool operation_write_base::post_write_impl( io::multiplexer* mux 
            , io::descriptor desc )
    {
#if defined( TCODE_WIN32 )
		_write = io_byte();
		return true;
#else
        _write = mux->write( desc , _buffer , _address , error() );
        if ( error() || _write >= 0 ) 
            return true;
        return false;
#endif
    }

    int operation_write_base::write_size( void ) {
        return _write;
    }

    tcode::io::ip::address& operation_write_base::address( void ){
        return _address; 
    }

	tcode::io::buffer& operation_write_base::buffer(void) {
		return _buffer;
	}

    bool operation_write_base::post_write( 
            io::operation* op_base 
            , io::multiplexer* mux 
            , io::descriptor desc )
    {
        operation_write_base* op( static_cast< operation_write_base* >(op_base));
        return op->post_write_impl( mux ,desc ); 
    }


}}}}
