#include "stdafx.h"
#include <tcode/io/uds/address.hpp>

namespace tcode { namespace io { namespace uds {
    
    address::address( void )
        : _len(sizeof(_address)){
        _address.sun_family = AF_UNIX;
    }
    
    address::~address( void ){
    }

    address::address( const struct sockaddr_un& addr ) 
        : _len(sizeof(sockaddr_un))
    {
        memcpy( &_address , &addr , _len );
    }

    address::address( const address& addr ) 
        : _len(addr._len)
    {
        memcpy( &_address , &addr._address , _len );
    }

    address::address( const char* file )
        :_len(sizeof(sockaddr_un))
    {
        _address.sun_family = AF_UNIX;
        strcpy( _address.sun_path , file );
    }
        
    struct sockaddr* address::sockaddr( void ) const{
        return reinterpret_cast< struct sockaddr* >(  
	    	const_cast< sockaddr_un* >( &_address ));
    }
   
    socklen_t      address::sockaddr_length( void ) const {
        return _len;    
    }
    
    socklen_t*      address::sockaddr_length_ptr( void ) {
        return &_len;    
    }

    std::string address::file( void )const{
    	return _address.sun_path;
    }
}}}
