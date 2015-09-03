#include "stdafx.h"
#include <tcode/io/ip/udp/socket.hpp>
#include <tcode/io/engine.hpp>

namespace tcode { namespace io { namespace ip { namespace udp {

    socket::socket( io::engine& e )
        : _engine( e ) 
        , _descriptor(nullptr){
    }

    socket::socket( socket&& s ) 
        : _engine( s._engine )
        , _descriptor(s._descriptor)
    {
       s._descriptor = nullptr;
    }
    
    socket::~socket( void ) {

    }

    bool socket::bind( const tcode::io::ip::address& addr ) {
        return _engine.mux().bind( _descriptor , addr );
    }

    void socket::close( void ) {
        _engine.mux().unbind( _descriptor );
    }
    
    io::descriptor& socket::descriptor( void ) {
        return _descriptor;
    }
}}}}
