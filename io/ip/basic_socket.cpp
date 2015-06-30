#include "stdafx.h"
#include "basic_socket.hpp"


namespace tcode { namespace io { namespace ip {

socket_handle_tcp::socket_handle_tcp( void )
{
	handle( invalid_socket );
}

socket_handle_tcp::~socket_handle_tcp( void )
{
	handle( invalid_socket );
}

bool socket_handle_tcp::open( int addressfamily ){
#ifdef TCODE_TARGET_WINDOWS
	handle(WSASocket( addressfamily ,SOCK_STREAM , IPPROTO_TCP , 
				nullptr , 0 , WSA_FLAG_OVERLAPPED ));
#else
	handle(::socket( addressfamily ,SOCK_STREAM , IPPROTO_TCP));
#endif
	return good();
}

void socket_handle_tcp::close( void ) {
	if ( good() ) {
		socket_type h = handle( invalid_socket );
		if ( h != invalid_socket ) {
#ifdef TCODE_TARGET_WINDOWS
			closesocket( h );
#else
			::close( h );   
#endif
		}
	}
}

bool socket_handle_tcp::shutdown( const int v ) {
	return ::shutdown( handle() , v ) != -1;
}

bool socket_handle_tcp::good( void ) {
	return handle() != invalid_socket;
}

socket_handle_udp::socket_handle_udp( void )
{
	handle( invalid_socket );
}

socket_handle_udp::~socket_handle_udp( void )
{
	handle( invalid_socket );
}

bool socket_handle_udp::open( int addressfamily ){
#ifdef TCODE_TARGET_WINDOWS
	handle(WSASocket( addressfamily ,SOCK_DGRAM , IPPROTO_UDP , 
				nullptr , 0 , WSA_FLAG_OVERLAPPED ));
#else
	handle(::socket( addressfamily ,SOCK_DGRAM , IPPROTO_UDP));
#endif
	return good();
}

void socket_handle_udp::close( void ) {
	if ( good() ) {
		socket_type h = handle( invalid_socket );
#ifdef TCODE_TARGET_WINDOWS
	    closesocket( h );
#else
        ::close( h );   
#endif
	}
}

bool socket_handle_udp::good( void ) {
	return handle() != invalid_socket;
}
}}}

/*
bool socket_api_enable_wait::wait_for_read( 
		const tcode::time_span& wait ) const
{
	if ( handle() == invalid_socket ) {
		return false;
	}
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET( handle() , &fdset);

    timeval tval;
	tval.tv_sec = static_cast<int>(wait.total_seconds());
	tval.tv_usec = wait.total_microseconds() % 1000 * 1000;
#ifdef TCODE_TARGET_WINDOWS
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

bool socket_api_enable_wait::wait_for_write(
		const tcode::time_span& wait ) const
{
	if ( handle() == invalid_socket ) {
		return false;
	}

	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET( handle() , &fdset);

    timeval tval;
	tval.tv_sec = static_cast<int>(wait.total_seconds());
	tval.tv_usec = wait.total_microseconds() % 1000 * 1000;
#ifdef TCODE_TARGET_WINDOWS
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

bool socket_api_enable_bind::bind( const tcode::net::ip::address& addr )
{
	return ::bind( handle() 
			, addr.sockaddr() 
			, addr.sockaddr_length()) != -1;
}

int socket_api_enable_tcp_io::recv( void* buf , int size 
	, const int flag ) const
{
	return ::recv( handle() , static_cast<char*>(buf) , size , flag );
}
int socket_api_enable_tcp_io::recv( void* buf , int size 
	, const int flag , const tcode::time_span& wait )
{
	return wait_for_read( wait ) ? 
		this->recv( buf , size , flag) : -1;
}
int socket_api_enable_tcp_io::recv_n( void* buf , int size
	 , const int flag  , const tcode::time_span& wait )
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
	
int socket_api_enable_tcp_io::send( void* buf , int size
	 , const int flag ) const
{
	return ::send( handle() , static_cast<char*>(buf) , size ,flag );
}

int socket_api_enable_tcp_io::send( void* buf , int size 
	, const int flag , const tcode::time_span& wait )
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
        if ( !wait_for_write( end - now ))
            return sendsize;
    }
    return sendsize;
}

tcode::net::ip::address socket_api_enable_address::local_address( void ) const
{
	tcode::net::ip::address address;
	int err = getsockname( handle() 
		, address.sockaddr()
		, address.sockaddr_length_ptr() );
	if ( err != 0 ) {
		
	}
	return address;
}

tcode::net::ip::address socket_api_enable_address::remote_address( void ) const
{
	tcode::net::ip::address address;
	int err = getpeername( handle() 
		, address.sockaddr()
		, address.sockaddr_length_ptr() );
	if ( err != 0 ) {
		
	}
    return address;
}



socket_api_enable_option::blocking::blocking( void ) : _option(0){

}

socket_api_enable_option::blocking::~blocking( void ) {

}

bool socket_api_enable_option::blocking::set_option( socket_type fd ) {
#ifdef TCODE_TARGET_WINDOWS
	return ioctlsocket( fd , FIONBIO , &_option ) != -1;
#else
    int x = fcntl( fd , F_GETFL , 0);
    return fcntl(fd , F_SETFL , x & ~O_NONBLOCK ) != -1;
#endif
}


socket_api_enable_option::non_blocking::non_blocking( void ) : _option(1){

}

socket_api_enable_option::non_blocking::~non_blocking( void ) {

}

bool socket_api_enable_option::non_blocking::set_option( socket_type fd ) {
#ifdef TCODE_TARGET_WINDOWS
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

socket_api_enable_option::linger 
	socket_api_enable_option::linger_remove_time_wait( linger_value(1,0));


bool socket_api_enable_accept::listen( const int pending ) {
	return ::listen( handle() , pending ) != -1;
}

socket_type socket_api_enable_accept::accept( tcode::net::ip::address& address ) const 
{
	return ::accept( handle() , address.sockaddr() , address.sockaddr_length_ptr());
}

socket_type socket_api_enable_accept::accept( tcode::net::ip::address& address 
	, const tcode::time_span& ts ) const 
{
	if ( wait_for_read(ts)) {
		return ::accept( handle() , address.sockaddr() , address.sockaddr_length_ptr());
	}
	return invalid_socket;
}

bool socket_api_enable_connect::connect( const tcode::net::ip::address& address )
{
	return ::connect( handle() , address.sockaddr() , address.sockaddr_length()) != -1;
}

bool socket_api_enable_connect::connect( const tcode::net::ip::address& address 
	, const tcode::time_span& wait )
{
	if ( connect( address )) {
		return true;
	}
#ifdef TCODE_TARGET_WINDOWS
    if ( WSAGetLastError() != WSAEWOULDBLOCK ) {
		return false;
	}
#else
    if ( errno != EINPROGRESS ){
        return false;
    }
#endif
    return wait_for_write( wait );
}




int socket_api_enable_udp_io::recvfrom(void* buf , int size 
	, tcode::net::ip::address& address
	, const int flag ) const
{
	return ::recvfrom( handle() ,  static_cast<char*>(buf) , size  , 
		flag , address.sockaddr() , address.sockaddr_length_ptr() );
}

int socket_api_enable_udp_io::recvfrom(void* buf , int size 
	, tcode::net::ip::address& address 
	, const int flag  
	, const tcode::time_span& ts ) const
{
	return wait_for_read( ts ) ? 
		this->recvfrom( buf , size , address , flag) : -1;
}
	
int socket_api_enable_udp_io::sendto(  void* buf , int size 
	, const tcode::net::ip::address& address 
	, const int flag ) const
{
	return ::sendto( handle() ,  static_cast<char*>(buf) , size  , 
		flag , address.sockaddr() , address.sockaddr_length() );
}


}}}

*/
