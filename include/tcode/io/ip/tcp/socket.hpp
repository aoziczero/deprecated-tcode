#ifndef __tcode_io_ip_tcp_socket_h__
#define __tcode_io_ip_tcp_socket_h__

#include <tcode/io/engine.hpp>
#include <tcode/io/ip/tcp/operation_connect.hpp>
#include <tcode/io/ip/tcp/operation_write.hpp>
#include <tcode/io/ip/tcp/operation_read.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class socket {
    public:
        socket( engine& e );

        socket( socket&& rhs );

        ~socket( void );
    
        template < typename Handler >
        void connect( const tcode::io::ip::address& addr 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_connect<Handler> ));
            new (ptr) operation_connect<Handler>( addr , handler );
            _engine.mux().connect( _descriptor , 
                    reinterpret_cast< operation_connect_base* >(ptr));
        }

        template < typename Handler >
        void write( const tcode::io::buffer& buf 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_write<Handler> ));
            new (ptr) operation_write<Handler>( buf , handler );
            _engine.mux().write( _descriptor
                    , reinterpret_cast< operation_write_base* >(ptr));
        }

        template < typename Handler >
        void read( const tcode::io::buffer& buf 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_read<Handler> ));
            new (ptr) operation_read<Handler>( buf , handler );
            _engine.mux().read( _descriptor
                    , reinterpret_cast< operation_read_base* >(ptr));
        }
        
        void close( void );
    private:
        engine& _engine;
        io::descriptor _descriptor;
    };

}}}}

#endif
