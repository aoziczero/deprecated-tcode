#ifndef __tcode_io_ip_tcp_socket_h__
#define __tcode_io_ip_tcp_socket_h__

#include <tcode/io/engine.hpp>
#include <tcode/io/ip/tcp/reactor_completion_handler_connect.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class socket {
    public:
        socket( engine& e );
        ~socket( void );
    
        template < typename Handler >
        void connect( const tcode::io::ip::address& addr 
                , const Handler& handler )
        {
            void* ptr = tcode::io::completion_handler_alloc( 
                    sizeof( reactor_completion_handler_connect<Handler>));
            new (ptr) reactor_completion_handler_connect<Handler>( addr , handler );
            _engine.impl().connect( _descriptor , 
                    reinterpret_cast< reactor_completion_handler_connect_base* >(ptr));
        }
    private:
        engine& _engine;
        engine::descriptor _descriptor;
    };

}}}}

#endif
