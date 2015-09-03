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
        socket( engine& e );

        //! move ctor
        socket( socket&& rhs );
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
            _engine.mux().connect( _descriptor , 
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
            _engine.mux().write( _descriptor
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
            _engine.mux().write( _descriptor
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
            _engine.mux().read( _descriptor
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
            _engine.mux().read( _descriptor
                    , reinterpret_cast< operation_read_base* >(ptr));
        }

        void close( void );
        io::descriptor& descriptor( void );
    private:
        engine& _engine;
        io::descriptor _descriptor;
    private:
        socket( void );
        socket( const socket& rhs );
        socket operator=( const socket& rhs );
        socket operator=( socket&& rhs );
    };

}}}}

#endif
