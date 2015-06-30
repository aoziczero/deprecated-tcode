//! 
//! \file	address.hpp
//! \brief	address
//! 

#ifndef __tcode_io_ip_address_h__
#define __tcode_io_ip_address_h__
#if defined(TCODE_TARGET_WINDOWS)

#elif defined(TCODE_TARGET_LINUX)
#include <netinet/in.h>
#include <netdb.h>
#elif defined(TCODE_TARGET_MACOSX)

#endif

namespace tcode { namespace io { namespace ip {

//! 
//! \class	address
//! \brief	address
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 18
//!
class address {
public:
	address( void );
	
	address( const address& rhs );
	address& operator=(const address& rhs);

	address( struct sockaddr* addr , int len );

	~address( void );

	struct sockaddr*	sockaddr( void ) const;
	const int		sockaddr_length( void ) const;
	void			sockaddr_length( int v );
	socklen_t*		sockaddr_length_ptr( void );


	std::string ip_address( void ) const;
	int		    port( void ) const;
	int			family( void ) const;

	std::string to_string(void) const;
public:
	static address from( const char* address , int port , int family = AF_INET );
	static address any( int port , int family = AF_INET );
private:
	struct sockaddr_storage _address;
	socklen_t _length;
};

}}}

#endif
