#ifndef __tcode_io_ip_tcp_socket_h__
#define __tcode_io_ip_tcp_socket_h__

#include <tcode/io/engine.hpp>
#include <tcode/io/ip/tcp/operation_connect.hpp>
#include <tcode/io/ip/tcp/operation_write.hpp>
#include <tcode/io/ip/tcp/operation_read.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    /*!
     * @class socket
     * @brief 
     */
    class socket {
    public:
        //! ctor
        socket( io::engine& e );

        //! move ctor
        socket( socket&& rhs );
        //! move assign
        socket& operator=( socket&& rhs );
        //! dtor
        ~socket( void );
    
        //!
        template < typename Handler >
        void connect( const tcode::io::ip::address& addr 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_connect<Handler> ));
            new (ptr) operation_connect<Handler>( addr , handler );
            engine().mux().connect( _descriptor , 
                    reinterpret_cast< operation_connect_base* >(ptr));
        }

        //!
        template < typename Handler >
        void write( const tcode::io::buffer& buf 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_write<Handler> ));
            new (ptr) operation_write<Handler>( buf , handler );
            engine().mux().write( _descriptor
                    , reinterpret_cast< operation_write_base* >(ptr));
        }
        
        //!
        template < typename Handler >
        void write( const std::vector<tcode::io::buffer>& bufs
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_writev<Handler> ));
            new (ptr) operation_writev<Handler>( bufs , handler );
            engine().mux().write( _descriptor
                    , reinterpret_cast< operation_write_base* >(ptr));
        }

        //!
        template < typename Handler >
        void read( const tcode::io::buffer& buf 
                , const Handler& handler
                , bool fixed = false )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_read<Handler> ));
            new (ptr) operation_read<Handler>( buf , handler ,fixed  );
            engine().mux().read( _descriptor
                    , reinterpret_cast< operation_read_base* >(ptr));
        }
        
        //!
        template < typename Handler >
        void read( const std::vector<tcode::io::buffer>& bufs
                , const Handler& handler 
                , bool fixed = false)
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_readv<Handler> ));
            new (ptr) operation_readv<Handler>( bufs , handler ,fixed );
            engine().mux().read( _descriptor
                    , reinterpret_cast< operation_read_base* >(ptr));
        }

        void close( void );
        io::descriptor& descriptor( void );
        tcode::io::engine& engine( void );
    private:
        io::engine* _engine;
        io::descriptor _descriptor;
    private:
        socket( void );
        socket( const socket& rhs );
        socket operator=( const socket& rhs );
    };

}}}}

#endif
