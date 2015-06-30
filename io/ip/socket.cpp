#include "stdafx.h"
#include "socket.hpp"
#include <common/time_stamp.hpp>

namespace tcode { namespace io { namespace ip {
/*

socket::category< AF_INET , SOCK_STREAM , IPPROTO_TCP>  socket::tcp_v4;
socket::category< AF_INET6 , SOCK_STREAM , IPPROTO_TCP> socket::tcp_v6;
socket::category< AF_INET , SOCK_DGRAM , IPPROTO_UDP>   socket::udp_v4;
socket::category< AF_INET6 , SOCK_DGRAM , IPPROTO_UDP>  socket::udp_v6;
#ifdef TCODE_OS_WINDOWS
socket::handle_type socket::invalid_socket = INVALID_SOCKET;
#else
socket::handle_type socket::invalid_socket = -1;
#endif

socket::socket( void )
	: _handle( invalid_socket ) {

}

socket::socket( socket::handle_type fd ) 
	: _handle( fd ) {

}

socket::socket( const socket& rhs ) 
	: _handle( rhs.handle() ) 
{
	
}

socket::socket( socket&& rhs )
	: _handle( rhs.handle() ) 
{
	rhs._handle = invalid_socket;
}

socket& socket::operator=( const socket& rhs ) {
	_handle = rhs.handle();
	return *this;
}

socket& socket::operator=( socket&& rhs ) {
	_handle = rhs.handle();
	rhs._handle = invalid_socket;
	return *this;
}

socket::~socket( void ){

}

void socket::swap( socket& rhs ) {
	std::swap( _handle , rhs._handle );
}


bool socket::open( const int addressFamily , const int type , const int protocol ) {
#ifdef TCODE_OS_WINDOWS
	_handle = WSASocket( addressFamily , 
						 type , 
						 protocol , 
						 nullptr , 
						 0 , 
						 WSA_FLAG_OVERLAPPED );
#else
    _handle = ::socket( addressFamily , 
						 type , 
						 protocol );
#endif
	return _handle != invalid_socket;
}

void socket::close( void ) {
	if ( _handle != invalid_socket ) {
#ifdef TCODE_OS_WINDOWS
	    closesocket( _handle );
#else
        ::close( _handle );   
#endif
    }
	_handle = invalid_socket;
}

bool socket::shutdown( const int type ) {
	return ::shutdown( handle() , type ) != -1;
}

bool socket::bind( const tcode::net::ip::address& addr ) {
	return ::bind( handle() 
			, addr.sockaddr() 
			, addr.sockaddr_length()) != -1;
}

bool socket::listen( const int pending ) {
	return ::listen( handle() , pending ) != -1;
}

tcode::net::ip::address socket::local_address( void ) const  {
	tcode::net::ip::address address;
	int err = getsockname( handle() 
		, address.sockaddr()
		, address.sockaddr_length_ptr() );
	if ( err != 0 ) {
		
	}
	return address;
}

tcode::net::ip::address socket::remote_address( void ) const {
	tcode::net::ip::address address;
	int err = getpeername( handle() 
		, address.sockaddr()
		, address.sockaddr_length_ptr() );
	if ( err != 0 ) {
		
	}
    return address;
}

bool socket::connect( const tcode::net::ip::address& address ) const {
	return ::connect( handle() , address.sockaddr() , address.sockaddr_length()) != -1;
}

socket socket::accept( tcode::net::ip::address& address ) const {
	socket fd(::accept( handle() , address.sockaddr() , address.sockaddr_length_ptr()));
	return fd;
}

int socket::recv( void* buf , int size  , const int flag ) const {
	return ::recv( handle() , static_cast<char*>(buf) , size , flag );
}

int socket::send( void* buf , int size , const int flag ) const {
	return ::send( handle() , static_cast<char*>(buf) , size ,flag );
}

int socket::recvfrom( void* buf 
	, int size 
	, tcode::net::ip::address& address 
	, const int flag ) const 
{
	return ::recvfrom( handle() ,  static_cast<char*>(buf) , size  , 
		flag , address.sockaddr() , address.sockaddr_length_ptr() );
}

int socket::sendto( void* buf 
	, int size 
	, const tcode::net::ip::address& address 
	, const int flag ) const 
{
	return ::sendto( handle() ,  static_cast<char*>(buf) , size  , 
		flag , address.sockaddr() , address.sockaddr_length() );
}

bool socket::wait_for_recv( const tcode::time_span& wait ) const {
	if ( handle() == invalid_socket ) {
		return false;
	}
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET( handle() , &fdset);

    timeval tval;
	tval.tv_sec = static_cast<int>(wait.total_seconds());
	tval.tv_usec = wait.total_microseconds() % 1000 * 1000;
#ifdef TCODE_OS_WINDOWS
	int result = select( 0 , &fdset , NULL , NULL , &tval );
	if( result <= 0 ) { // -1 error 0 timeout
		return false;
	}	
#else
    while( true ) {
        int ret = select( handle() + 1 ,&fdset , NULL ,  NULL , &tval );
        if ( ret > 0 )  break;
        if ( ret == -1 && errno == EINTR ) continue;
        if( result <= 0 ) {  // -1 error 0 timeout
			return false;
		}
    }
#endif
	return FD_ISSET( handle() , &fdset ) != 0;
}

bool socket::wait_for_send( const tcode::time_span& wait ) const {
	if ( handle() == invalid_socket ) {
		return false;
	}

	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET( handle() , &fdset);

    timeval tval;
	tval.tv_sec = static_cast<int>(wait.total_seconds());
	tval.tv_usec = wait.total_microseconds() % 1000 * 1000;
#ifdef TCODE_OS_WINDOWS
	int result = select( 0 , NULL , &fdset , NULL , &tval );
	if( result <= 0 ) {  // -1 error 0 timeout
		return false;
	}
#else
    while( true ) {
        int ret = select( handle() + 1 , NULL , &fdset ,  NULL , &tval );
        if ( ret > 0 )  break;
        if ( ret == -1 && errno == EINTR ) continue;
        if( result <= 0 ) {  // -1 error 0 timeout
			return false;
		}
    }    
#endif
	return FD_ISSET( handle() , &fdset ) != 0;
}

bool socket::connect( const tcode::net::ip::address& address ,  const tcode::time_span& wait ) {
	if ( connect( address )) {
		return true;
	}
#ifdef TCODE_OS_WINDOWS
    if ( WSAGetLastError() != WSAEWOULDBLOCK ) {
		return false;
	}
#else
    if ( errno != EINPROGRESS ){
        return false;
    }
#endif
    return wait_for_send( wait );
}

int socket::send( void* buf , int size  
		, const int flag 
		, const tcode::time_span& wait ) 
{
	tcode::time_stamp end = tcode::time_stamp::now() + wait;
    int sendsize = 0;
    unsigned char* buffer = static_cast< unsigned char* >(buf);
    int reqsize = size;
    while ( sendsize < reqsize ) {
		tcode::time_stamp now = tcode::time_stamp::now();
		if ( now > end ) {
			break;
		}
        int ret = this->send( buffer + sendsize 
					, reqsize - sendsize 
					, flag );
        if ( ret <= 0 ) {
			return sendsize;
		}
        sendsize += ret;
        if ( !wait_for_send( end - now ))
            return sendsize;
    }
    return sendsize;
}

int  socket::recv( void* buf , int size 
		, const int flag 
		, const tcode::time_span& wait )
{
    return wait_for_recv( wait ) ? 
		this->recv( buf , size , flag) : -1;
}

int socket::recv_n( void* buf , int size  
		, const int flag 
		, const tcode::time_span& wait ) 
{
	tcode::time_stamp end = tcode::time_stamp::now() + wait;
    int recvsize = 0;
    unsigned char* buffer = static_cast< unsigned char* >(buf);
    int reqsize = size;
    while ( recvsize < reqsize ) {
		tcode::time_stamp now = tcode::time_stamp::now();
		if ( now > end ) {
			break;
		}
        int ret = recv( buffer + recvsize 
					, reqsize - recvsize  
					, flag
					, end - now );
        if ( ret <= 0 ) {
            return recvsize;
        }
        recvsize += ret;		
    }
    return recvsize;
}


socket::handle_type socket::handle( void ) const {
    return _handle;
}

socket::handle_type socket::handle( socket::handle_type fd ) {
	socket::handle_type old = _handle;
	_handle = fd;
	return old;
}

bool socket::good( void ) const  {
	return _handle != invalid_socket;
}

bool operator==( const socket& rhs , const socket& lhs ) {
    return rhs.handle() == lhs.handle();
}

bool operator!=( const socket& rhs , const socket& lhs ) {
    return rhs.handle() != lhs.handle();
}


socket::option::blocking::blocking( void ) : _option(0){

}

socket::option::blocking::~blocking( void ) {

}

bool socket::option::blocking::set_option( socket::handle_type fd ) {
#ifdef TCODE_OS_WINDOWS
	return ioctlsocket( fd , FIONBIO , &_option ) != -1;
#else
    int x = fcntl( fd , F_GETFL , 0);
    return fcntl(fd , F_SETFL , x & ~O_NONBLOCK ) != -1;
#endif
}


socket::option::non_blocking::non_blocking( void ) : _option(1){

}

socket::option::non_blocking::~non_blocking( void ) {

}

bool socket::option::non_blocking::set_option( socket::handle_type fd ) {
#ifdef TCODE_OS_WINDOWS
	return ioctlsocket( fd , FIONBIO , &_option ) != -1;
#else
    int x = fcntl( fd , F_GETFL , 0);
    return fcntl(fd , F_SETFL , x | O_NONBLOCK )!= -1;
#endif
}

namespace {
	::linger linger_value( int on_off , int liv ){
		::linger li;
		li.l_onoff = on_off;
		li.l_linger = liv;
		return li;
	}
}

socket::option::linger socket::option::linger_remove_time_wait( linger_value(1,0));
*/
}}}