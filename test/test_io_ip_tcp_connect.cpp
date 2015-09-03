#include "stdafx.h"
#include <tcode/io/ip/address.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

char request[] = "GET / HTTP/1.1\r\n\r\n";

void on_read(
        tcode::io::ip::tcp::socket& fd 
        , const std::error_code& ec 
        , int read 
        , char* buf )
{
    gout << "Read:" << read << ":"  << buf  << gendl;
    delete [] buf;
    fd.close();
}
 
void on_send( 
        tcode::io::ip::tcp::socket& fd 
        , const std::error_code& ec 
        , int write )
{
    gout << "Write:" << write << ":"  << ec.message() << gendl;
}

void on_connect( tcode::io::ip::tcp::socket& fd
       , const std::error_code& ec 
       , const tcode::io::ip::address& addr )
{
    gout << ec.message() << gendl;
    char* buf = new char[512];
    //fd.read_fixed
    fd.read( tcode::io::buffer( buf , 512)
            , [&fd , buf] ( const std::error_code& ec 
                , int read ) 
            {
                on_read( fd , ec , read , buf );
            });

    fd.write( tcode::io::buffer( request , strlen( request ))
            , [&fd] ( const std::error_code& ec 
                , int write )
            {
                on_send( fd , ec , write );
            });
}

TEST( tcode_io_ip_tcp_socket , connect ) {
    tcode::io::engine e;
    tcode::io::ip::tcp::socket fd( e );
    tcode::io::ip::address google( AF_INET 
            , tcode::io::ip::address::dns("google.co.kr")
            , 80 );
    
    fd.connect( google ,[&fd]( const std::error_code& ec
                , const tcode::io::ip::address& addr ) {
                        on_connect( fd , ec , addr );
                    } );
    e.run();
}


