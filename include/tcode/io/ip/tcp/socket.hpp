#ifndef __tcode_io_ip_tcp_socket_h__
#define __tcode_io_ip_tcp_socket_h__

namespace tcode { namespace io { 

    class engine;
    
namespace ip { namespace tcp {

    class socket {
    public:
        socket( engine& e );
        ~socket( void );
    private:
        engine& _engine;
    };

}}}}

#endif
