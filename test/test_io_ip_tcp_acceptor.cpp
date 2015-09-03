#include "stdafx.h"
#include <tcode/io/ip/tcp/acceptor.hpp>

void on_accept( 
        tcode::io::ip::tcp::socket& fd 
        , const std::error_code& ec ){
    gout << ec.message() << gendl;
    fd.close();    
}

TEST( tcode_io_ip_acceptor , listen ) {
    tcode::io::engine e;
    tcode::io::ip::tcp::acceptor acceptor(e);
    acceptor.listen(
            tcode::io::ip::address(
            tcode::io::ip::address::any( 12345 )));

    tcode::io::ip::tcp::socket fd(e);
    acceptor.accept( fd ,[&fd , &acceptor ]( const std::error_code& ec 
                , const tcode::io::ip::address& addr )
            {
                on_accept( fd , ec );
                acceptor.close();
            });

    tcode::io::ip::tcp::socket conn( e );
    conn.connect( tcode::io::ip::address( "127.0.0.1" , 12345 )
            , [&conn]( const std::error_code& ec 
                , const tcode::io::ip::address& addr )
            {
                gout << "conn" << ec.message() << gendl;
                conn.close();
            });
    e.run();
}
