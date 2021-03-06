#ifndef __tcode_io_ip_tcp_operation_connect__
#define __tcode_io_ip_tcp_operation_connect__

#include <tcode/error.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    /*!
     * @class operation_connect_base
     * @brief 
     */
    class operation_connect_base 
        : public tcode::io::operation
    {
    public:
        //! ctor
        operation_connect_base( tcode::operation::execute_handler fn
                , const tcode::io::ip::address& addr );
            
        //! dtor
        ~operation_connect_base( void );

        //! 
        static bool post_connect( io::operation* op
                , io::multiplexer* mux 
                , io::descriptor desc );
        //! 
        tcode::io::ip::address& address( void );
    private:
        //! 
        tcode::io::ip::address _address;
    };


    /*!
     * @class operation_connect
     * @brief 
     */
    template < typename Handler >
    class operation_connect 
        : public operation_connect_base 
    {
    public:
        operation_connect( 
                const tcode::io::ip::address& addr 
                , const Handler& handler )
            : operation_connect_base( &operation_connect::complete , addr )
            , _handler( handler )
        {
        }

        ~operation_connect( void ){
        }

        static void complete( tcode::operation* op ) {
            operation_connect* impl(
                    static_cast< operation_connect* >(op));
            Handler h( std::move( impl->_handler ));
            std::error_code ec = impl->error();
            tcode::io::ip::address addr( impl->address());
            impl->~operation_connect(); 
            tcode::operation::free(  impl );
            h( ec , addr );
        }
    private:
        Handler _handler;
    };
}}}}

#endif
