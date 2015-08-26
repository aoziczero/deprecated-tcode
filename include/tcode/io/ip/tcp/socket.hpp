#ifndef __tcode_io_ip_tcp_socket_h__
#define __tcode_io_ip_tcp_socket_h__

#include <tcode/io/ip/ip.hpp>

#include <tcode/function.hpp>

namespace tcode { namespace io { 

    class engine;
    
namespace ip { namespace tcp {

    class socket {
    public:
        socket( engine& e );
        ~socket( void );

        bool open( int family );
        void close( const tcode::function< void () >& onclose );


        void do_close( ip::socket_type fd );

        ip::socket_type native_handle( void );
        ip::socket_type native_handle( ip::socket_type handle );
    private:
        engine& _engine;
        std::atomic< ip::socket_type > _handle;
    };

}}}}

#endif
