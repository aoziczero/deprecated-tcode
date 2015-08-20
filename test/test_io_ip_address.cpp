#include "stdafx.h"
#include <tcode/io/ip/address.hpp>

TEST( tcode_io_ip_address , ctor ){
    tcode::io::ip::address addr;
}

TEST( tcode_io_ip_address , any ){
    tcode::io::ip::address::any any( AF_INET ,  7543  );
    tcode::io::ip::address bindaddr( any );
    ASSERT_STREQ( bindaddr.ip().c_str() , "0.0.0.0");
    ASSERT_EQ( bindaddr.port() , 7543 );
    ASSERT_EQ( bindaddr.family() , AF_INET);
    gout << bindaddr.ip() << gendl;
}

TEST( tcode_io_ip_address , from_ip ) {
    tcode::io::ip::address target( AF_INET ,  "192.168.0.1" , 5432 );
}
