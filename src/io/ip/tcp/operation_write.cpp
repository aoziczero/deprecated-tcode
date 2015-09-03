#include "stdafx.h"
#include <tcode/io/ip/tcp/operation_write.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    operation_write_base::operation_write_base( 
            tcode::operation::execute_handler fn
            , tcode::io::buffer* buffer 
            , int buffercnt )
        : tcode::io::operation( fn , &operation_write_base::post_write )
        , _buffer( buffer ) , _buffer_count( buffercnt ) , _write(0)
    {
    }

    operation_write_base::~operation_write_base( void ){
    }

    bool operation_write_base::post_write_impl( io::multiplexer* mux 
            , io::descriptor desc )
    {
        int write = mux->writev( desc , _buffer , _buffer_count , error() );

        if ( error() ) 
            return true;
        if ( write < 0 ) 
            return false;

        int writeskip = write;
        int remain = 0;

        for ( int i = 0 ; i < _buffer_count ; ++i ){
            if ( writeskip > 0 ) {
                int per_skip = std::min( writeskip , _buffer[i].len() );
                _buffer[i].skip( per_skip );
                writeskip -= per_skip;
            }
            remain += _buffer[i].len();
        }

        _write += write;

        return remain == 0;
    }

    void operation_write_base::buffers( tcode::io::buffer* buf , int cnt ){
        _buffer = buf;
        _buffer_count = cnt;
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
