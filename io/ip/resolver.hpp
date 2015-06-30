//! 
//! \file	address.hpp
//! \brief	address
//! 

#ifndef __tcode_io_ip_resolver_h__
#define __tcode_io_ip_resolver_h__

#include <io/ip/address.hpp>

#include <vector>

namespace tcode { namespace io { namespace ip {

//! 
//! \class	resolver
//! \brief	resolver
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 20
//!
class resolver {
public:
	resolver( void );
	~resolver( void );

	//! ip / doamin 등을 address 로 변환
	//! local address 는 nullptr 로...
	std::vector< tcode::io::ip::address > resolve( 
		const char* addr 
		, int port 
		, int family = AF_UNSPEC 
		, int type = SOCK_STREAM
		, int flags = AI_PASSIVE);
private:
		
};

}}}

#endif