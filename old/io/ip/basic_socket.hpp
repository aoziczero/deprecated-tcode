#ifndef __tcode_io_ip_basic_socket_handle_h__
#define __tcode_io_ip_basic_socket_handle_h__

#include <common/basic_handle.hpp>

#include <io/ip/address.hpp>
#include <common/time_span.hpp>
#include <common/time_stamp.hpp>

#if defined( TCODE_TARGET_LINUX )
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#endif

namespace tcode { namespace io { namespace ip {

#ifdef TCODE_TARGET_WINDOWS	
typedef SOCKET socket_type;	
#else
typedef int socket_type;
#endif


#ifdef TCODE_TARGET_WINDOWS	
static socket_type invalid_socket = INVALID_SOCKET;
static socket_type socket_error = SOCKET_ERROR;
#else
static socket_type invalid_socket =-1;
static socket_type socket_error = invalid_socket;
#endif


class socket_handle_tcp 
	: public tcode::basic_handle< socket_type >
{
public:
	socket_handle_tcp( void );

	bool open( int addressfamily );
	void close( void );	

	bool shutdown( const int v );
	
	bool good( void );
protected:
	~socket_handle_tcp( void );
};

template< typename parent_type >
class socket_api_enable_option
	: public parent_type
{
protected:
	~socket_api_enable_option(){}
public:
	template < typename T_option >
	bool set_option( T_option& opt ) {
		return opt.set_option( parent_type::handle() );
	}

	template < typename T_option >
	bool get_option( T_option& opt ) {
		return opt.get_option( parent_type::handle() );
	}

	template < typename T , int level , int opt >
	class value_option {
	public:
		value_option( void ) {
		}
		explicit value_option( const T& t ) : _option(t) {
		}
		~value_option( void ) {
		}
		bool set_option( socket_type fd ) {
			return setsockopt( fd , 
					    level , 
					    opt , 
					    reinterpret_cast< char*>(&_option) , 
					    sizeof( _option ) ) != socket_error;
		}
		bool get_option( socket_type fd ) {
			socklen_t size = sizeof( _option );
			return getsockopt( fd,
							    level , 
							    opt , 
							    reinterpret_cast< char*>(&_option) ,
							    &size ) != socket_error;
		}

		T& value( void ) {
			return _option;
		}

		void value( const T& value ) {
			_option = value;
		}

	private:
		T _option;
	};
    
	class blocking {
	public:
		blocking( void ) 
			: _option(0){
		}
		bool set_option( socket_type fd ) {
			#ifdef TCODE_TARGET_WINDOWS
			return ioctlsocket( fd , FIONBIO , &_option ) != -1;
			#else
			int x = fcntl( fd , F_GETFL , 0);
			return fcntl(fd , F_SETFL , x & ~O_NONBLOCK ) != -1;
			#endif
		}
	private:
		unsigned long _option;
	};

	class non_blocking {
	public:
		non_blocking( void )
			: _option(1){
		}
		bool set_option( socket_type fd ){
#ifdef TCODE_TARGET_WINDOWS
			return ioctlsocket( fd , FIONBIO , &_option ) != -1;
#else
			int x = fcntl( fd , F_GETFL , 0);
			return fcntl(fd , F_SETFL , x | O_NONBLOCK )!= -1;
#endif
		}
	private:
		unsigned long _option;
	};
	
    typedef value_option< int       , SOL_SOCKET	, SO_REUSEADDR			> reuse_address;
    typedef value_option< int       , SOL_SOCKET	, SO_BROADCAST			> broad_cast;
    typedef value_option< int		, SOL_SOCKET	, SO_RCVBUF				> recv_buffer;
    typedef value_option< int		, SOL_SOCKET	, SO_RCVTIMEO			> recv_time;
    typedef value_option< int		, SOL_SOCKET	, SO_SNDBUF				> send_buffer;
    typedef value_option< int		, SOL_SOCKET	, SO_SNDTIMEO			> send_time;
	typedef value_option< int		, SOL_SOCKET	, SO_ERROR 			    > error; 
    typedef value_option< struct linger	, SOL_SOCKET	, SO_LINGER				> linger;
		
	//static linger linger_remove_time_wait(1,0);

	typedef value_option< uint8_t	, IPPROTO_IP	, IP_MULTICAST_LOOP		> multi_cast_loop;
	typedef value_option< uint8_t	, IPPROTO_IP	, IP_MULTICAST_TTL		> multi_cast_ttl;
	typedef value_option< in_addr	, IPPROTO_IP	, IP_MULTICAST_IF		> multi_cast_if;
	typedef value_option< ip_mreq	, IPPROTO_IP	, IP_ADD_MEMBERSHIP		> multi_cast_join;
	typedef value_option< ip_mreq	, IPPROTO_IP	, IP_DROP_MEMBERSHIP	> multi_cast_drop;


	typedef value_option< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_LOOP	> multi_cast_loop_ipv6;
	typedef value_option< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_HOPS	> multi_cast_ttl_ipv6;
	typedef value_option< in6_addr	, IPPROTO_IPV6	, IPV6_MULTICAST_IF		> multi_cast_if_ipv6;
	typedef value_option< ipv6_mreq	, IPPROTO_IPV6	, IPV6_ADD_MEMBERSHIP	> multi_cast_join_ipv6;
	typedef value_option< ipv6_mreq	, IPPROTO_IPV6	, IPV6_DROP_MEMBERSHIP	> multi_cast_drop_ipv6;
		
	typedef value_option< int       , IPPROTO_TCP	, TCP_NODELAY		> tcp_no_delay;

#if defined(_WIN32) || defined(_WIN64)	
    typedef value_option< BOOL		, SOL_SOCKET	, SO_CONDITIONAL_ACCEPT		> conditional_accept;
    typedef value_option< int		, SOL_SOCKET	, SO_CONNECT_TIME			> connect_time;
    typedef value_option< SOCKET	, SOL_SOCKET	, SO_UPDATE_ACCEPT_CONTEXT	> update_accept_context;
#endif
};


template< typename parent_type >
class socket_api_enable_wait
	: public parent_type
{
protected:
	~socket_api_enable_wait(){}
public:
	bool wait_for_read( const tcode::time_span& wait ) const{
		if ( parent_type::handle() == invalid_socket ) {
			return false;
		}
		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET( parent_type::handle() , &fdset);

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
			int ret = select( parent_type::handle() + 1 ,&fdset , NULL ,  NULL , &tval );
			if ( ret > 0 )  break;
			if ( ret == -1 && errno == EINTR ) continue;
			if( ret <= 0 ) {  // -1 error 0 timeout
				return false;
			}
		}
	#endif
		return FD_ISSET( parent_type::handle() , &fdset ) != 0;
	}
	bool wait_for_write(const tcode::time_span& wait ) const{
		if ( parent_type::handle() == invalid_socket ) {
			return false;
		}

		fd_set fdset;
		FD_ZERO(&fdset);
		FD_SET( parent_type::handle() , &fdset);

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
			int ret = select( parent_type::handle() + 1 , NULL , &fdset ,  NULL , &tval );
			if ( ret > 0 )  break;
			if ( ret == -1 && errno == EINTR ) continue;
			if( ret <= 0 ) {  // -1 error 0 timeout
				return false;
			}
		}    
	#endif
		return FD_ISSET( parent_type::handle() , &fdset ) != 0;
	}
};

template< typename parent_type >
class socket_api_enable_bind 	
	: public parent_type
{
protected:
	~socket_api_enable_bind(){}
public:
	bool bind( const tcode::io::ip::address& addr ){
		return ::bind( parent_type::handle()
			, addr.sockaddr() 
			, addr.sockaddr_length()) != -1;
	}
};

template< typename parent_type >
class socket_api_enable_tcp_io
	:  public parent_type
{
protected:
	~socket_api_enable_tcp_io(){}
public:
	int recv( void* buf , int size , const int flag = 0 ) const{
		return ::recv( parent_type::handle() , static_cast<char*>(buf) , size , flag );
	}
	int recv( void* buf , int size , const int flag , const tcode::time_span& wait ) {
		return parent_type::wait_for_read( wait ) ?
			this->recv( buf , size , flag) : -1;
	}
    int recv_n( void* buf , int size , const int flag  , const tcode::time_span& wait ){
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
	
		
	int send( void* buf , int size
		 , const int flag = 0 ) const
	{
		return ::send( parent_type::handle() , static_cast<char*>(buf) , size ,flag );
	}

	int send( void* buf , int size 
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
			if ( !parent_type::wait_for_write( end - now ))
				return sendsize;
		}
		return sendsize;
	}
};

template< typename parent_type >
class socket_api_enable_address
	: public parent_type
{
protected:
	~socket_api_enable_address(){}
public:
	tcode::io::ip::address local_address( void ) const
	{
		tcode::io::ip::address addr;
		int err = getsockname( parent_type::handle()
			, addr.sockaddr()
			, addr.sockaddr_length_ptr() );
		if ( err != 0 ) {
		
		}
		return addr;
	}

	tcode::io::ip::address remote_address( void ) const
	{
		tcode::io::ip::address addr;
		int err = getpeername( parent_type::handle()
			, addr.sockaddr()
			, addr.sockaddr_length_ptr() );
		if ( err != 0 ) {
		
		}
		return addr;
	}
};

template< typename parent_type >
class socket_api_enable_accept
	: public parent_type
{
protected:
	~socket_api_enable_accept(){}
public:	
	bool listen( const int pending  = SOMAXCONN ) {
		return ::listen( parent_type::handle() , pending ) != -1;
	}

	socket_type accept( tcode::io::ip::address& address ) const 
	{
		return ::accept( parent_type::handle() , address.sockaddr() , address.sockaddr_length_ptr());
	}

	socket_type accept( tcode::io::ip::address& address 
		, const tcode::time_span& ts ) const 
	{
		if ( parent_type::wait_for_read(ts)) {
			return ::accept( parent_type::handle() , address.sockaddr() , address.sockaddr_length_ptr());
		}
		return invalid_socket;
	}
};

template< typename parent_type >
class socket_api_enable_connect
	: public parent_type
{
protected:
	~socket_api_enable_connect(){}
public:
	bool connect( const tcode::io::ip::address& address )
	{
		return ::connect( parent_type::handle() , address.sockaddr() , address.sockaddr_length()) != -1;
	}

	bool connect( const tcode::io::ip::address& address 
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
		return parent_type::wait_for_write( wait );
	}
};

class socket_handle_udp 
	: public tcode::basic_handle< socket_type >
{
public:
	socket_handle_udp( void );

	bool open( int addressfamily );
	void close( void );	
	
	bool good( void );
protected:
	~socket_handle_udp( void );
};

template< typename parent_type >
class socket_api_enable_udp_io
	: public parent_type
{
protected:
	~socket_api_enable_udp_io(){}
public:
	int recvfrom(void* buf , int size 
		, tcode::io::ip::address& address
		, const int flag = 0 ) const
	{
		return ::recvfrom( parent_type::handle() ,  static_cast<char*>(buf) , size  ,
			flag , address.sockaddr() , address.sockaddr_length_ptr() );
	}

	int recvfrom(void* buf , int size 
		, tcode::io::ip::address& address 
		, const int flag  
		, const tcode::time_span& ts ) const
	{
		return parent_type::wait_for_read( ts ) ?
			this->recvfrom( buf , size , address , flag) : -1;
	}
	
	int sendto(  void* buf , int size 
		, const tcode::io::ip::address& address 
		, const int flag = 0) const
	{
		return ::sendto( parent_type::handle() ,  static_cast<char*>(buf) , size  ,
			flag , address.sockaddr() , address.sockaddr_length() );
	}

};

template < typename p1 >
class socket_host ;

template < typename p1 , template <typename parent_type> class p2 >
class socket_host2 : public p2< p1 >
{

};

template < typename p1 
	, template <typename parent_type> class p2 
	, template <typename parent_type> class p3
	>
class socket_host3 : public p3< p2< p1 > >
{
};

template < typename p1 
	, template <typename parent_type> class p2 
	, template <typename parent_type> class p3
	, template <typename parent_type> class p4
	>
class socket_host4 : public p4< p3< p2< p1 > > >
{
};

template < typename p1 
	, template <typename parent_type> class p2 
	, template <typename parent_type> class p3
	, template <typename parent_type> class p4
	, template <typename parent_type> class p5
	>
class socket_host5 : public p5< p4< p3< p2< p1 > > > >
{
};

template < typename p1 
	, template <typename parent_type> class p2 
	, template <typename parent_type> class p3
	, template <typename parent_type> class p4
	, template <typename parent_type> class p5
	, template <typename parent_type> class p6
	>
class socket_host6 : public p6< p5< p4< p3< p2< p1 > > > > >
{
};

typedef tcode::io::ip::socket_host2<
			tcode::io::ip::socket_handle_tcp
		, 	tcode::io::ip::socket_api_enable_option
		> tcp_holder;

typedef tcode::io::ip::socket_host6< 
			tcode::io::ip::socket_handle_tcp
		, 	tcode::io::ip::socket_api_enable_option	
		,	tcode::io::ip::socket_api_enable_address
		,	tcode::io::ip::socket_api_enable_bind
		,   tcode::io::ip::socket_api_enable_wait
		,	tcode::io::ip::socket_api_enable_tcp_io
		> tcp_base;

typedef tcode::io::ip::socket_host5< 
			tcode::io::ip::socket_handle_udp
		, 	tcode::io::ip::socket_api_enable_option	
		,	tcode::io::ip::socket_api_enable_bind
		,   tcode::io::ip::socket_api_enable_wait
		,	tcode::io::ip::socket_api_enable_udp_io
		> udp_base;

typedef tcode::io::ip::socket_host4<
			tcode::io::ip::socket_handle_tcp 
		,	tcode::io::ip::socket_api_enable_option
		,	tcode::io::ip::socket_api_enable_bind
		,	tcode::io::ip::socket_api_enable_accept 
		> accept_base;


typedef tcode::io::ip::socket_host5<
			tcode::io::ip::socket_handle_tcp 
		,	tcode::io::ip::socket_api_enable_option
		,	tcode::io::ip::socket_api_enable_bind
		,   tcode::io::ip::socket_api_enable_wait
		,	tcode::io::ip::socket_api_enable_connect
		> connect_base;

}}}

#endif

