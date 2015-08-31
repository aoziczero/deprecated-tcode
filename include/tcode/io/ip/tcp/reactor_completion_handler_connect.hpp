#ifndef __tcode_io_ip_tco_reactor_completion_handler_connect_h__
#define __tcode_io_ip_tco_reactor_completion_handler_connect_h__

#include <tcode/error.hpp>
#include <tcode/io/ip/address.hpp>
#include <tcode/io/reactor_completion_handler.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    class reactor_completion_handler_connect_base 
        : public tcode::io::reactor_completion_handler
    {
    public:
        reactor_completion_handler_connect_base( 
                base_type::completion_function fn
                , const tcode::io::ip::address& addr )
            : reactor_completion_handler( fn 
                    , reactor_completion_handler_connect_base::reactor_connect )
              , _address(addr)
        {

        }

        ~reactor_completion_handler_connect_base( void ){}

        static bool reactor_connect( reactor_completion_handler* h , 
                void* handle )
        {
            return true;
        }
        tcode::io::ip::address& address( void ) {
            return _address;
        }
    private:
        tcode::io::ip::address _address;
    };

    template < typename Handler >
    class reactor_completion_handler_connect 
        : public reactor_completion_handler_connect_base
    {
    public:
        reactor_completion_handler_connect( 
                const tcode::io::ip::address& addr 
                , const Handler& handler )
            : reactor_completion_handler_connect_base( 
                    reactor_completion_handler_connect::complete , addr )
              , _handler( handler )
        {
        }

        ~reactor_completion_handler_connect( void ){
        }

        static void complete( completion_handler* ch , void* handle ) {
            reactor_completion_handler_connect* impl(
                    static_cast< reactor_completion_handler_connect* >(ch));
            std::error_code ec;
            if ( !handle ) {
                ec = tcode::error_aborted;     
            }
            Handler h( std::move( impl->_handler ));
            tcode::io::ip::address addr( impl->address());
            impl->~reactor_completion_handler_connect(); 
            tcode::io::completion_handler_free( impl );
            h( ec , addr );
        }
    private:
        Handler _handler;
    };
}}}}

#endif
