#include "stdafx.h"
#include <tcode/io/ip/tcp/operation_read.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    operation_read_base::operation_read_base( 
            tcode::operation::execute_handler fn
            , tcode::io::buffer* buffer
            , int buffercnt
            , bool fixed_len )
        : tcode::io::operation( fn ,
                fixed_len ? 
                    &operation_read_base::post_read_fixed_len :
                    &operation_read_base::post_read )
        , _buffer( buffer ) , _buffer_count( buffercnt ), _read(0) 
    {
    }

    operation_read_base::~operation_read_base( void ){
    }

    bool operation_read_base::post_read_impl( io::multiplexer* mux 
            , io::descriptor desc )
    {
#if defined( TCODE_WIN32 )
		if ( error() )
			return true;
		_read = io_byte();
		if ( _read == 0 )
			error() = tcode::error_disconnected;
		return true;
#else
        _read = mux->readv( desc , _buffer , _buffer_count , error() );
        if ( error() || _read >= 0 )
            return true;
        return false;
#endif
	}

    bool operation_read_base::post_read_impl_fixed_len( io::multiplexer* mux
            , io::descriptor desc )
    {
#if defined( TCODE_WIN32 )
		if ( error() )
			return true;

		if ( io_byte() == 0 ) {
			error() = tcode::error_disconnected;
			return true;
		}
		int readskip = io_byte();
        int remain = 0;
        for ( int i = 0 ; i < _buffer_count ; ++i ){
            if ( readskip > 0 ) {
                int per_skip = std::min( readskip , _buffer[i].len() );
                _buffer[i].skip( per_skip );
                readskip -= per_skip;
            }
            remain += _buffer[i].len();
        }
        _read += io_byte();
        if ( remain == 0 )
			return true;
		mux->read( desc , this );
		return false;
#else
        int read = mux->readv( desc , _buffer , _buffer_count , error() );

        if ( error() ) 
            return true;
        if ( read < 0 ) 
            return false;
        
        int readskip = read;
        int remain = 0;
        for ( int i = 0 ; i < _buffer_count ; ++i ){
            if ( readskip > 0 ) {
                int per_skip = std::min( readskip , _buffer[i].len() );
                _buffer[i].skip( per_skip );
                readskip -= per_skip;
            }
            remain += _buffer[i].len();
        }
        _read += read;
        return remain == 0;
#endif
    }

    void operation_read_base::buffers( tcode::io::buffer* buf , int cnt ){
        _buffer = buf;
        _buffer_count = cnt;
    }

    int operation_read_base::read_size( void ) {
        return _read;
    }

	tcode::io::buffer* operation_read_base::buffers(void) {
		return _buffer;
	}
	int	operation_read_base::buffers_count(void) {
		return _buffer_count;
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
