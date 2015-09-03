#ifndef __tcode_io_ip_tcp_operation_accept__
#define __tcode_io_ip_tcp_operation_accept__

#include <tcode/error.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class socket;

    /*!
     * @class operation_accept_base
     * @brief 
     */
    class operation_accept_base 
        : public tcode::io::operation
    {
    public:
        //! ctor
        operation_accept_base( tcode::operation::execute_handler fn
                , tcode::io::ip::tcp::socket& fd );
            
        //! dtor
        ~operation_accept_base( void );

        bool post_accept_impl( io::multiplexer* mux 
                , io::descriptor desc );
        //! 
        static bool post_accept( io::operation* op
                , io::multiplexer* mux 
                , io::descriptor desc );
        //! 
        tcode::io::ip::address& address( void );
    private:
        //! 
        tcode::io::ip::tcp::socket& _fd;
        //! 
        tcode::io::ip::address _address;
    };


    /*!
     * @class operation_accept
     * @brief 
     */
    template < typename Handler >
    class operation_accept 
        : public operation_accept_base 
    {
    public:
        operation_accept( 
                tcode::io::ip::tcp::socket& fd 
                , const Handler& handler )
            : operation_accept_base( &operation_accept::complete , fd )
            , _handler( handler )
        {
        }

        ~operation_accept( void ){
        }

        static void complete( tcode::operation* op ) {
            operation_accept* impl(
                    static_cast< operation_accept* >(op));
            Handler h( std::move( impl->_handler ));
            std::error_code ec = impl->error();
            tcode::io::ip::address addr(impl->address());
            impl->~operation_accept(); 
            tcode::operation::free(  impl );
            h( ec , addr );
        }
    private:
        Handler _handler;
    };
}}}}

#endif
