#include "stdafx.h"
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    socket::socket( io::engine& e )
        : _engine( e ) {
    }
    
    socket::~socket( void ) {

    }

    bool socket::open( int family ){
        native_handle( ::socket( family , SOCK_STREAM , IPPROTO_TCP ));
        return native_handle() != tcode::io::ip::invalid_socket;
    }
        
    void socket::close( const tcode::function< void () >& onclose ){
        ip::socket_type old = native_handle( tcode::io::ip::invalid_socket );
        if ( old != tcode::io::ip::invalid_socket )
            _engine.post( 
                [this,old,onclose] {
                    do_close(old); 
                    onclose(); 
                });
    }

    void socket::do_close( ip::socket_type fd ) {
        _engine.unbind( fd );
        ::close( fd );
        // todo drain reqs;
    }

    ip::socket_type socket::native_handle( void ) {
        return _handle.load();
    }

    ip::socket_type socket::native_handle( ip::socket_type handle ){
        return _handle.exchange( handle );
    }


}}}}
