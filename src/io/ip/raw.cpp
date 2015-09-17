#include "stdafx.h"
#include <tcode/io/ip/raw.hpp>

namespace tcode { namespace io { namespace ip { namespace raw {

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
    
    socket::~socket( void ) {

    }

    bool socket::open( int af , int type , int proto ) {
        return engine().mux().open( _descriptor , af , type , proto );
    }
    
    int socket::read( tcode::io::buffer& buf 
                , tcode::io::ip::address& addr ) {
        std::error_code e;
        return engine().mux().read( _descriptor
                , buf , addr , e );
    }

    int socket::write( const tcode::io::buffer& buf 
                , const tcode::io::ip::address& addr ){
        std::error_code e;
        return engine().mux().write( _descriptor
                , buf , addr , e );
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
