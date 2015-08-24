#include "stdafx.h"
#include "resolver.hpp"

namespace tcode { namespace io { namespace ip {

resolver::resolver( void ){}
resolver::~resolver( void ){}

std::vector< tcode::io::ip::address > 
resolver::resolve( const char* addr 
		, int port 
		, int family
		, int type 
		, int flags
)
{
	std::vector<tcode::io::ip::address> addrs;

	struct addrinfo hint = {0,};
	struct addrinfo* res = nullptr;
	struct addrinfo* temp = nullptr;

	hint.ai_family = family;
	hint.ai_socktype = type;
	hint.ai_flags = flags;
	char port_string[12];
#if defined( _WIN32 )
	sprintf_s( port_string , "%d" , port );
#else
	sprintf( port_string , "%d" , port );
#endif
	int err = getaddrinfo( addr, port_string , &hint, &res);
    if ( err == 0) {
		temp = res;
		while (temp) {
			tcode::io::ip::address a( temp->ai_addr, static_cast<int>(temp->ai_addrlen));	
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
