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
    
    int socket::read( tcode::io::buffer& buf 
                , tcode::io::ip::address& addr ) {
        std::error_code e;
        return _engine.mux().read( _descriptor
                , buf , addr , e );
    }

    int socket::write( const tcode::io::buffer& buf 
                , const tcode::io::ip::address& addr ){
        std::error_code e;
        return _engine.mux().write( _descriptor
                , buf , addr , e );
    }
        
    void socket::close( void ) {
        _engine.mux().unbind( _descriptor );
    }
    
    io::descriptor& socket::descriptor( void ) {
        return _descriptor;
    }
}}}}
