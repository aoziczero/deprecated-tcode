#ifndef __tcode_io_ip_tcp_acceptor_h__
#define __tcode_io_ip_tcp_acceptor_h__

#include <tcode/tcode.hpp>
#include <tcode/io/io_define.hpp>
#include <tcode/io/ip/address.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/ip/tcp/operation_accept.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    /*!
     * @class acceptor
     */
    class acceptor{
    public:
        //! ctor
        acceptor( io::engine& en );
        //! dtor
        ~acceptor( void );

        bool listen( const tcode::io::ip::address& addr );
        void close( void );
       
        template < typename Handler >
        void accept( tcode::io::ip::tcp::socket& fd 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( operation_accept<Handler> ));
            new (ptr) operation_accept<Handler>( fd , handler );
            _engine.mux().accept( _descriptor , 
                    reinterpret_cast< operation_accept_base* >(ptr));
        }
    private:
        io::engine& _engine;
        io::descriptor _descriptor;
    };

}}}}

#endif
