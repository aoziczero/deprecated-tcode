#include "stdafx.h"
#include <tcode/io/ip/udp/socket.hpp>

TEST( tcode_io_ip_udp , rw ){
    tcode::io::engine e;
    tcode::io::ip::udp::socket s(e);
    tcode::io::ip::udp::socket c(e);

    s.bind( tcode::io::ip::address(
                tcode::io::ip::address::any( 23456 )));
    char buf[1024];
    s.read( tcode::io::buffer( buf , 1024 )
            , [&s]( const std::error_code& e 
                , int read 
                , const tcode::io::ip::address& addr )
            {
                gout << "S:" << e.message() << ":" << read << gendl;
                s.close();
            });
    char hello[] = "Hello!";
    c.write( tcode::io::buffer( hello , strlen(hello))
            , tcode::io::ip::address( "127.0.0.1" , 23456 )
            , [&c]( const std::error_code& e 
                , int write
                , const tcode::io::ip::address& addr )
            {
                gout << "C:" << e.message() << ":" << write << gendl;
                c.close();
            });
    e.run();
}
