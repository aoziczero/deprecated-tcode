#include "stdafx.h"
#include <tcode/io/ip/tcp/operation_read.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    operation_read_base::operation_read_base( 
            tcode::operation::execute_handler fn
            , const tcode::io::buffer& buf 
            , bool fixed_len )
        : tcode::io::operation( fn ,
                fixed_len ? 
                    &operation_read_base::post_read_fixed_len :
                    &operation_read_base::post_read )
        , _buffer( buf ) , _read(0) 
    {
    }

    operation_read_base::~operation_read_base( void ){
    }

    bool operation_read_base::post_read_impl( io::multiplexer* mux 
            , io::descriptor desc )
    {
        _read = mux->readv( desc , &_buffer , 1 , error() );
        if ( error() ) 
            return true;
        return _read > 0;
    }

    bool operation_read_base::post_read_impl_fixed_len( io::multiplexer* mux
            , io::descriptor desc )
    {
        io::buffer buf( _buffer.buf() + _read , _buffer.len() - _read );
        int r = mux->readv( desc , &buf, 1 , error());
        if ( error() )
            return true;
        if ( r >= 0 )
            _read += r;
        return _read == _buffer.len();
    }

    int operation_read_base::read_size( void ) {
        return _read;
    }

    bool operation_read_base::post_read( 
            io::operation* op_base 
            , io::multiplexer* mux 
            , io::descriptor desc )
    {
        operation_read_base* op( static_cast< operation_read_base* >(op_base));
        return op->post_read_impl( mux ,desc ); 
    }
    
    bool operation_read_base::post_read_fixed_len( io::operation* op_base 
            , io::multiplexer* mux
            , io::descriptor desc )
    {
        operation_read_base* op( static_cast< operation_read_base* >(op_base));
        return op->post_read_impl_fixed_len( mux ,desc ); 
    }

}}}}
