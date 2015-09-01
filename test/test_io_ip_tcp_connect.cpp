#include "stdafx.h"
#include <tcode/io/ip/address.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

void on_connect( tcode::io::ip::tcp::socket& fd
       , const std::error_code& ec 
       , const tcode::io::ip::address& addr )
{
    gout << ec.message() << gendl;
    fd.close();
}

TEST( tcode_io_ip_tcp_socket , connect ) {

    tcode::io::engine e;
    tcode::io::ip::tcp::socket fd( e );
    tcode::io::ip::address google( AF_INET 
            , tcode::io::ip::address::dns("google.co.kr")
            , 80 );
    
    fd.connect( google ,[&fd]( const std::error_code& ec
                , const tcode::io::ip::address& addr ) {
                        gout << ec.message() << gendl;
                        on_connect( fd , ec , addr );
                    } );
    e.run();
}
