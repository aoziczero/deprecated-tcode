#include "stdafx.h"
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    socket::socket( io::engine& e )
        : _engine( e ) {
    }

    socket::socket( socket&& s ) 
        : _engine( s._engine )
        , _descriptor(s._descriptor)
    {
       s._descriptor = nullptr;
    }
    
    socket::~socket( void ) {

    }

    void socket::close( void ) {
        _engine.mux().unbind( _descriptor );
    }
    
    io::descriptor& socket::descriptor( void ) {
        return _descriptor;
    }
}}}}
