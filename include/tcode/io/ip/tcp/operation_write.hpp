#ifndef __tcode_io_ip_tcp_operation_write__
#define __tcode_io_ip_tcp_operation_write__

#include <tcode/error.hpp>
#include <tcode/io/operation.hpp>
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class operation_write_base 
        : public tcode::io::operation
    {
    public:
        operation_write_base( tcode::operation::execute_handler fn
                , const tcode::io::ip::address& addr )
            : tcode::io::operation( fn , &operation_write_base::post_write )
              , _address(addr)
        {

        }

        ~operation_write_base( void ){}

        static bool post_write( io::operation* op , 
                void* handle )
        {
        }
    private:
        tcode::io::ip::address _address;
    };

    template < typename Handler >
    class operation_write 
        : public operation_write_base 
    {
    public:
        operation_write( 
                const tcode::io::ip::address& addr 
                , const Handler& handler )
            : operation_write_base( &operation_write::complete , addr )
            , _handler( handler )
        {
        }

        ~operation_write( void ){
        }

        static void complete( tcode::operation* op ) {
            operation_write* impl(
                    static_cast< operation_write* >(op));
            Handler h( std::move( impl->_handler ));
            std::error_code ec = impl->error();
            tcode::io::ip::address addr( impl->address());
            impl->~operation_write(); 
            tcode::operation::free(  impl );
            h( ec , addr );
        }
    private:
        Handler _handler;
    };
}}}}

#endif
