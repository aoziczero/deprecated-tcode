// tests.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <tcode/io/engine.hpp>
#include <tcode/io/ip/raw.hpp>
#include <tcode/log/log.hpp>

char buffer[1024];
tcode::io::ip::raw::socket* fd;
int read_raw( const std::error_code& ec , int n , 
        const tcode::io::ip::address& addr ) {
    LOG_D("arp","read %d" , n );
    fd->read( tcode::io::buffer( buffer , 1024 )
            , &read_raw );
}

int main( int argc , char* argv[]) {
	
    LOG_T("ECHO" , "start");
	tcode::io::engine e;
    
    tcode::io::ip::raw::socket lfd(e);
    fd = &lfd;

    if (fd->open( AF_PACKET , SOCK_RAW , htons(0x0003))){
        LOG_T( "arp" , "socket open success" );
        
        fd->read( tcode::io::buffer( buffer , 1024 ) 
                , &read_raw );
        
        e.run();
    }
    
	return 0;
}


