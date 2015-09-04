#include "stdafx.h"
#include <tcode/io/ip/tcp/acceptor.hpp>
#include <tcode/io/engine.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    acceptor::acceptor( io::engine& en )
        : _engine( en )
        , _descriptor( nullptr )
    {
    }

    acceptor::~acceptor( void ){
    }

    bool acceptor::listen( const tcode::io::ip::address& addr ){
		_address = addr;
        return _engine.mux().listen( _descriptor , addr ); 
    }

    void acceptor::close( void ) {
        _engine.mux().unbind( _descriptor );
    }

}}}}
