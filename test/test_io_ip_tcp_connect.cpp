#include "stdafx.h"
#include <tcode/io/ip/address.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <tcode/io/engine.hpp>

char request[] = "GET / HTTP/1.1\r\n\r\n";
char get[] = "GET";
char space[] = " "; 
char req[] = "/";
char http_ver[] = "HTTP/1.1";
char end[] = "\r\n\r\n";


char* read_1;
char* read_2;

void on_read(
        tcode::io::ip::tcp::socket& fd 
        , const std::error_code& ec 
        , int read)
{
    gout << "Read:" << read << ":"  << read_1 << gendl;
    if ( read > 200 ) 
        gout << "Read2:" << read_2 << gendl;
    delete [] read_1;
    delete [] read_2;
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
    read_1 = new char[200];
    read_2 = new char[200];
    //fd.read_fixed
    std::vector< tcode::io::buffer > read_bufs;
    read_bufs.push_back( tcode::io::buffer( read_1 , 200 ));
    read_bufs.push_back( tcode::io::buffer( read_2 , 200 ));
    fd.read( read_bufs 
            //tcode::io::buffer( buf , 512)
            , [&fd
            //, buf
              ] ( const std::error_code& ec 
                , int read ) 
            {
                on_read( fd , ec , read );
            },true);

    std::vector< tcode::io::buffer > bufs;
    bufs.push_back( tcode::io::buffer( get , strlen(get)));
    bufs.push_back( tcode::io::buffer( space , strlen(space)));
    bufs.push_back( tcode::io::buffer( req, strlen(req)));
    bufs.push_back( tcode::io::buffer( space , strlen(space)));
    bufs.push_back( tcode::io::buffer( http_ver, strlen(http_ver)));
    bufs.push_back( tcode::io::buffer( end, strlen(end)));
    fd.write( bufs 
            //tcode::io::buffer( request , strlen( request ))
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


