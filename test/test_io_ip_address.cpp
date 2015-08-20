#include "stdafx.h"
#include <tcode/io/ip/address.hpp>

TEST( tcode_io_ip_address , ctor ){
    tcode::io::ip::address addr;
}

TEST( tcode_io_ip_address , any ){
    tcode::io::ip::address::any any( 7543  );
    tcode::io::ip::address bindaddr( any );
    ASSERT_STREQ( bindaddr.ip().c_str() , "0.0.0.0");
    ASSERT_EQ( bindaddr.port() , 7543 );
    ASSERT_EQ( bindaddr.family() , AF_INET);
    gout << bindaddr.ip() << gendl;
}

TEST( tcode_io_ip_address , from_ip ) {
    tcode::io::ip::address target( "192.168.0.1" , 5432 );
    ASSERT_STREQ( target.ip().c_str() , "192.168.0.1");
    ASSERT_EQ( target.port() , 5432 );
    ASSERT_EQ( target.family() , AF_INET);
}


TEST( tcode_io_ip_address , from_dns_v4) {
    tcode::io::ip::address target( AF_INET
            , tcode::io::ip::address::dns( "google.co.kr" )
            , 5431 );

    gout << target.ip() << gendl;
}

TEST( tcode_io_ip_address , from_dns_v6) {
    tcode::io::ip::address target( AF_INET6
            , tcode::io::ip::address::dns( "google.co.kr" )
            , 5431 );

    gout << target.ip() << gendl;
}

TEST( tcode_io_ip_address , from_dns_list_unspec ) {
    auto addrs = tcode::io::ip::address::from_dns(
            AF_UNSPEC 
            , tcode::io::ip::address::dns( "google.co.kr" )
            , 5431 );

    auto it = addrs.begin();
    while ( it != addrs.end()){
        gout << it->ip() << gendl;
        ++it;
    }
}

TEST( tcode_io_ip_address , from_dns_list_v4_v6) {
    auto addrs = tcode::io::ip::address::from_dns(
            AF_INET
            , tcode::io::ip::address::dns( "google.co.kr" )
            , 5431 );

    auto it = addrs.begin();
    while ( it != addrs.end()){
        gout << "v4:" << it->ip() << gendl;
        ++it;
    }
    addrs = tcode::io::ip::address::from_dns(
            AF_INET6
            , tcode::io::ip::address::dns( "google.co.kr" )
            , 5431 );

    it = addrs.begin();
    while ( it != addrs.end()){
        gout << "v6:" << it->ip() << gendl;
        ++it;
    }

}
