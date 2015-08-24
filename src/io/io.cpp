#include "stdafx.h"
#include <tcode/io/io.hpp>

#include <unistd.h>
#include <fcntl.h>

namespace tcode { namespace io {

    handle::handle( void )
        : _handle( -1 )
    {
    }

    handle::handle(handle::native_type fd )
        : _handle(fd)
    {
    }
          
    handle::handle( handle&& rhs )
        : _handle(std::move(rhs._handle))
    {
    }

    handle::handle( const handle& h )
        : _handle( h._handle )
    {
    }

    handle handle::operator=( native_type fd ) {
        _handle = fd;
        return *this;
    }

    handle handle::operator=( const handle& rhs ) {
        _handle = rhs._handle;
        return *this;
    }

    handle handle::operator=( handle&& rhs ){
        _handle = std::move( rhs._handle );
        return *this;
    }

    handle::~handle( void ){
        _handle = -1;
    }

    void handle::close( void ) {
        ::close(_handle);
    }

    bool handle::good( void ) const {
        return _handle != -1;
    }

    handle::native_type handle::native_handle( void ) {
        return _handle;
    }
namespace option {
    
    bool block::set_option( handle::native_type fd )
    {
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt & ~O_NONBLOCK ) != -1; 
    }
 
    bool nonblock::set_option( handle::native_type fd )
    {
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt | O_NONBLOCK ) != -1; 
    }
}


}}
