#include "stdafx.h"
#include <tcode/io/ip/tcp/connector.hpp>
#include <tcode/time/timespan.hpp>

TEST( tcode_io_ip_tcp_connector , connect_timeout ) {
    tcode::io::ip::tcp::connector c;

    tcode::io::ip::address google( AF_INET , 
            , tcode::io::ip::address::dns("google.co.kr")
            , 80 );

    int fd = c.connect( google , tcode::timespan::seconds(20));
    ASSERT_TRUE( fd != -1 );
    close(fd);
}
