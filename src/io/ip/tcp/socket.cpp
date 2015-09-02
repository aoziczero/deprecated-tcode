#include "stdafx.h"
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    socket::socket( io::engine& e )
        : _engine( e ) {
    }

    socket::socket( socket&& s ) 
        : _engine( s._engine )
        , _descriptor( std::move(s._descriptor))
    {
        
    }
    
    socket::~socket( void ) {

    }

    void socket::close( void ) {
        _engine.mux().unbind( _descriptor );
    }
    
}}}}
