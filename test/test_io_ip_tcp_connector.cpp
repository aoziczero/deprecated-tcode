#include "stdafx.h"
#include <tcode/io/ip/tcp/connector.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

tcode::io::ip::address google( AF_INET 
            , tcode::io::ip::address::dns("google.co.kr")
            , 80 );
/*

TEST( tcode_io_ip_tcp_connector , connect_timeout ) {
    tcode::io::ip::tcp::connector c;
    tcode::io::ip::address google2( AF_INET 
            , tcode::io::ip::address::dns("google.co.kr")
            , 81 );

    tcode::io::ip::socket_type fd = c.connect( google2 , tcode::timespan::seconds(2));
    ASSERT_TRUE( fd == tcode::io::ip::invalid_socket );
}


TEST( tcode_io_ip_tcp_connector , connect ) {
    tcode::io::ip::tcp::connector c;

    tcode::io::ip::socket_type fd = c.connect( google , tcode::timespan::seconds(20));
    ASSERT_TRUE( fd != -1 );
    tcode::io::ip::close( fd );
}
*/

void on_connect( const std::error_code& ec ) {
    
}

TEST( tcode_io_ip_tcp_connector , connect ){
    tcode::io::engine e;
    tcode::io::ip::tcp::socket fd(e);
    tcode::io::ip::tcp::connector c(fd);
   // c.connect( google , on_connect );
   // e.run( tcode::timespan::seconds(10));
//    tcode::io::engine e;
//    tcode::io::ip::tcp::connector c(e);
//    c.connect( 
}
