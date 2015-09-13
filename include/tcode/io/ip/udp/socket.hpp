#ifndef __tcode_io_ip_udp_socket_h__
#define __tcode_io_ip_udp_socket_h__

#include <tcode/io/engine.hpp>
#include <tcode/io/ip/udp/operation_write.hpp>
#include <tcode/io/ip/udp/operation_read.hpp>

namespace tcode { namespace io { namespace ip { namespace udp {

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

        bool bind( const tcode::io::ip::address& addr );

        int write( const tcode::io::buffer& buf 
                , const tcode::io::ip::address& addr );
        
        int read( tcode::io::buffer& buf 
                , tcode::io::ip::address& addr );
        
        //!
        template < typename Handler >
        void write( const tcode::io::buffer& buf 
                , const tcode::io::ip::address& addr 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_write<Handler> ));
            new (ptr) operation_write<Handler>( buf , addr , handler );
            _engine.mux().write( _descriptor
                    , reinterpret_cast< operation_write_base* >(ptr));
        }
       
        //!
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
