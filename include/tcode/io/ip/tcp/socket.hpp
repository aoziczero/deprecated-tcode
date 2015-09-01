#ifndef __tcode_io_ip_tcp_socket_h__
#define __tcode_io_ip_tcp_socket_h__

#include <tcode/io/engine.hpp>
#include <tcode/io/ip/tcp/operation_connect.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class socket {
    public:
        socket( engine& e );
        ~socket( void );
    
        template < typename Handler >
        void connect( const tcode::io::ip::address& addr 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_connect<Handler> ));
            new (ptr) operation_connect<Handler>( addr , handler );
            _engine.impl().connect( _descriptor , 
                    reinterpret_cast< operation_connect_base* >(ptr));
        }

        void close( void );
    private:
        engine& _engine;
        engine::descriptor _descriptor;
    };

}}}}

#endif
