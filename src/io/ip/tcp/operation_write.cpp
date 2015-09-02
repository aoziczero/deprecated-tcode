#include "stdafx.h"
#include <tcode/io/ip/tcp/operation_write.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    operation_write_base::operation_write_base( 
            tcode::operation::execute_handler fn
            , const tcode::io::buffer& buf )
        : tcode::io::operation( fn , &operation_write_base::post_write )
        , _buffer( buf ) , _write(0)
    {
    }

    operation_write_base::~operation_write_base( void ){
    }

    bool operation_write_base::post_write_impl( io::multiplexer* mux 
            , io::descriptor desc )
    {
        tcode::io::buffer write_buf( _buffer.buf() + _write 
                , _buffer.len() - _write );
        int write = mux->writev( desc , &write_buf , 1 , error() );

        if ( error() ) 
            return true;
        
        _write += write;
        return _write == _buffer.len();
    }

    int operation_write_base::write_size( void ) {
        return _write;
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
