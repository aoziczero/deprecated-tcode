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
    tcode::io::ip::raw::ether_header* ehdr =
        reinterpret_cast< tcode::io::ip::raw::ether_header*>(buffer);

    //LOG_T( "arp" , "%04x" , ehdr->ether_type );
    if ( ehdr->ether_type == 0x0608 ){
        tcode::io::ip::raw::arphdr* arph=
            reinterpret_cast< tcode::io::ip::raw::arphdr*>( ehdr + 1 );    
        LOG_D("arp" , "arp packet src %d.%d.%d.%d(%02x:%02x:%02x:%02x:%02x:%02x) dst %d.%d.%d.%d(%02x:%02x:%02x:%02x:%02x:%02x)"
                , arph->ar_sha[0] , arph->ar_sha[1] , arph->ar_sha[2] , arph->ar_sha[3] 
                , arph->ar_sip[0] , arph->ar_sip[1] , arph->ar_sip[2] , arph->ar_sip[3] , arph->ar_sip[4] , arph->ar_sip[5]
                , arph->ar_tha[0] , arph->ar_tha[1] , arph->ar_tha[2] , arph->ar_tha[3] 
                , arph->ar_tip[0] , arph->ar_tip[1] , arph->ar_tip[2] , arph->ar_tip[3] , arph->ar_tip[4] , arph->ar_tip[5]
                );
        
    }
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


