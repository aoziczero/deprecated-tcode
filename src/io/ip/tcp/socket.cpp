#include "stdafx.h"
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    socket::socket( io::engine& e )
        : _engine( &e )
        , _descriptor(nullptr){
    }

    socket::socket( socket&& s ) 
        : _engine( s._engine )
        , _descriptor(s._descriptor)
    {
        s._engine = nullptr;
        s._descriptor = nullptr;
    }
    
    socket& socket::operator=( socket&& s ) 
    {
        _engine = s._engine;
        _descriptor = s._descriptor;
        s._engine = nullptr;
        s._descriptor = nullptr;
        return *this;
    }
    socket::~socket( void ) {

    }

    void socket::close( void ) {
        engine().mux().unbind( _descriptor );
    }
    
    io::descriptor& socket::descriptor( void ) {
        return _descriptor;
    }

    io::engine& socket::engine( void ) {
        return *_engine;
    }

}}}}
