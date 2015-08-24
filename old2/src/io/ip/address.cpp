#include "stdafx.h"
#include <tcode/io/ip/address.hpp>

namespace tcode { namespace io { namespace ip {
namespace detail {

const char* inet_ntop(int af, const void *src, char *dst, socklen_t cnt) {
    sockaddr_storage storage;
    int size = 0;
	switch( af ) {
	case AF_INET:{
			sockaddr_in* addr = reinterpret_cast< sockaddr_in* >(&storage);
			addr->sin_family = AF_INET;
			size = sizeof( sockaddr_in );
			memcpy(&addr->sin_addr, src, sizeof(in_addr));
		}
		break;
	case AF_INET6:{
			sockaddr_in6* addr = reinterpret_cast< sockaddr_in6* >(&storage);
			addr->sin6_family = AF_INET6;
			size = sizeof( sockaddr_in6 );
			memcpy(&addr->sin6_addr, src, sizeof(in6_addr));
		}
		break;
	default:
		return dst;
	}
#if defined( _WIN32 )
    if ( GetNameInfoA(  
#else
    if ( getnameinfo(
#endif                
                        reinterpret_cast< struct sockaddr*>(&storage) , 
                        size , 
                        dst , 
                        cnt , 
                        nullptr , 
                        0 , 
                        NI_NUMERICHOST ) != 0 ) {
    }
    return dst;
}

bool inet_pton( int af
	, const char* addr
	, int port
	, address& outaddr)
{
	struct addrinfo hints = {0,};
	struct addrinfo* res = nullptr;
	hints.ai_family = af;
	char port_string[12];
#if defined( _WIN32 )
	sprintf_s( port_string , "%d" , port );
#else
	sprintf( port_string , "%d" , port );
#endif
	int err = getaddrinfo( addr , port_string , &hints, &res);
    if ( err != 0) {
		return false;
    }
    if (res) {
		outaddr = address( res->ai_addr , static_cast<int>(res->ai_addrlen));
		freeaddrinfo(res);
		return true;
	}
	return false;;
}

}
    address::address( void )
        : _len(sizeof(_address)){
    }
    
    address::~address( void ){
    }

    address::address( const any& any ) 
        : _len(sizeof(sockaddr_in))
    {
        struct sockaddr_in addr;
        memset(&addr , 0x00 , _len );
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(any.port);
        addr.sin_family = AF_INET; 
        memcpy( &_address , &addr , _len );
    }

    address::address( struct sockaddr* addr , int len ) 
        : _len(len)
    {
        memcpy( &_address , addr , _len );
    }

    address::address( const char* ip , const uint16_t port )
        :_len(sizeof(sockaddr_in))
    {
        struct sockaddr_in addr;
        memset(&addr , 0x00 , _len );
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);
        addr.sin_family = AF_INET;
        memcpy( &_address , &addr , _len );
    }
        
    address::address( const int af , const dns& dns , const uint16_t port )
        : _len( sizeof(_address))
    {
        detail::inet_pton( af , dns.domain , port , *this );
    }
    struct sockaddr* address::sockaddr( void ) const{
        return reinterpret_cast< struct sockaddr* >(  
	    	const_cast< sockaddr_storage* >( &_address ));
    }
   
    socklen_t      address::sockaddr_length( void ) const {
        return _len;    
    }
    
    socklen_t*      address::sockaddr_length_ptr( void ) {
        return &_len;    
    }

    std::string address::ip( void ){
    	char buffer[4096] = {0,};
    	std::string ip = detail::inet_ntop(
              family() 		
            , &(((struct sockaddr_in*)sockaddr())->sin_addr) 
    		, buffer
    		, 4096);
    	return ip;
    }
    int address::port( void ) const {
    	return ntohs(((struct sockaddr_in*)sockaddr())->sin_port );
    }

    int	address::family( void ) const {
	    return ((struct sockaddr_in*)sockaddr())->sin_family;
    }
    address::any::any( int p )
        : port(p)     
    {
    }

    address::dns::dns( const char* d )
        : domain(d)
    {
    }
    
    std::vector<address> address::from_dns( const int af
                , const dns& dns 
                , const uint16_t port 
                , const int type
                , const int flags )
    {
    	std::vector<address> addrs;

        struct addrinfo hint = {0,};
        struct addrinfo* res = nullptr;
        struct addrinfo* temp = nullptr;

        hint.ai_family = af;
        hint.ai_socktype = type;
        hint.ai_flags = flags;
        char port_string[12];
#if defined( _WIN32 )
        sprintf_s( port_string , "%d" , port );
#else
        sprintf( port_string , "%d" , port );
#endif
        int err = getaddrinfo( dns.domain , port_string , &hint, &res);
        if ( err == 0 ) {
            temp = res;
            while (temp) {
                address a( temp->ai_addr
                        , static_cast<int>(temp->ai_addrlen));	
                addrs.push_back( a );
                temp = temp->ai_next;
            }
            if ( res ) {
                freeaddrinfo(res);
            }
        }
        return addrs;
    }
}}}
