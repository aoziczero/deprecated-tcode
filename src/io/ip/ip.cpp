#include "stdafx.h"
#include <tcode/io/ip/ip.hpp>
#if defined( TCODE_WIN32 )

#else
#include <unistd.h>
#include <fcntl.h>
#endif
namespace tcode { namespace io { namespace ip {
    handle::native_type handle::invalid = 
#if defined( TCODE_WIN32 )
        INVALID_SOCKET;
#else
        -1;
#endif
    handle::native_type handle::error = 
#if defined( TCODE_WIN32 )
        SOCKET_ERROR;
#else
        -1;
#endif

    handle::handle( void )
        : _handle( handle::invalid )
    {
    }

    handle::handle( native_type fd )
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

    handle::~handle( void ) {
        _handle = handle::invalid;
    }

    void handle::close( void ){
#if defined( TCODE_WIN32 )
        closesocket( _handle );
#else
        ::close( _handle );
#endif
        _handle = handle::invalid;
    }

    bool handle::good( void ) const {
        return _handle != handle::invalid;
    }

    handle::native_type handle::native_handle( void ) {
        return _handle;
    }

namespace option {

    bool block::set_option( handle::native_type fd )
    {
#if defined( TCODE_WIN32 )
        unsigned long opt = 0;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
#else
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt & ~O_NONBLOCK ) != -1; 
#endif
    }
 
    bool nonblock::set_option( handle::native_type fd )
    {
#if defined( TCODE_WIN32 )
        unsigned long opt = 1;
        return ioctlsocket( fd , FIONBIO , &opt ) != -1;
#else
        int opt = fcntl( fd , F_GETFL , 0 );
        return fcntl( fd , F_SETFL , opt | O_NONBLOCK ) != -1; 
#endif
    }


    linger_remove_time_wait::linger_remove_time_wait( void )
    {
        option().l_onoff = 1;
        option().l_linger = 0;
    }
}


}}}
