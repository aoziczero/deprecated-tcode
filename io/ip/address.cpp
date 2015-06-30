#include "stdafx.h"
#include "address.hpp"

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
	: _length( sizeof( sockaddr_storage))
{
	memset( sockaddr() , 0x00 , sizeof( sockaddr_storage ));
}

address::address( const address& rhs ) {
	memcpy( sockaddr() , rhs.sockaddr() , rhs.sockaddr_length());
	_length = rhs.sockaddr_length();
}

address& address::operator=(const address& rhs) {
	memcpy( sockaddr() , rhs.sockaddr() , rhs.sockaddr_length());
	_length = rhs.sockaddr_length();
	return *this;
}

address::address( struct sockaddr* addr , int len ) {
	memcpy( sockaddr() , addr , len);
	_length = len;
}

address::~address( void ){

}

struct sockaddr*	address::sockaddr( void ) const{
	return reinterpret_cast< struct sockaddr* >(  
		const_cast< sockaddr_storage* >( &_address ));
}

const int		address::sockaddr_length( void ) const{
	return _length;
}

void			address::sockaddr_length( int v ) {
	_length = v;
}

socklen_t*		address::sockaddr_length_ptr( void ){
	return &_length;
}


std::string address::ip_address( void ) const {
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


std::string address::to_string(void) const {
	return ip_address() + ":" + std::to_string(port());
}

address address::any( int port , int family ) {
	if ( family == AF_INET ) {
		struct sockaddr_in addr;
		memset( &addr , 0x00 , sizeof( addr ));
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = htons(port);
		addr.sin_family = AF_INET;	
		return address( reinterpret_cast<struct sockaddr*>(&addr) , sizeof( addr ));
	}
	return address();
}

address address::from( const char* address , int port , int family  ){
	ip::address addr;
	if ( detail::inet_pton( family , address , port , addr ) ) {
		return addr;
	} 
	return any( port , family );
}

}}}
