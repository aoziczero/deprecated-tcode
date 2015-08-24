#include "stdafx.h"
#include <tcode/io/ip/tcp/connector.hpp>
#include <tcode/io/ip/option.hpp>
#include <tcode/io/ip/simple_select.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp {

    ip::socket_type connector::connect( const ip::address& addr 
        , const tcode::timespan& ts ) {
        ip::socket_type fd = ip::socket( addr.family() ,SOCK_STREAM , IPPROTO_TCP );        
        if ( fd != ip::invalid_socket ) {
            tcode::io::ip::option::non_blocking nb;
            if ( nb.set_option( fd ) ){
#if defined( TCODE_WIN32 )
                if ( ::connect( fd 
                            , addr.sockaddr()
                            , addr.sockaddr_length()) != -1 )
                {
                    if ( ip::selector::wait_for_write( fd , ts )){
                        return fd;
                    }
                }
#else
                int r;
                do {
                    r = ::connect(fd 
                            , addr.sockaddr()
                            , addr.sockaddr_length());
                }while((r==-1)&&(errno==EINTR));
                if (( r == 0 ) || errno == EINPROGRESS ) {
                    if ( ip::selector::wait_for_write( fd , ts )){
                        return fd;
                    }
                }
#endif                
            }
            ip::close( fd );
        }
        return ip::invalid_socket;
    }

}}}}
