#ifndef __tcode_io_option_h__
#define __tcode_io_option_h__
#if defined( TCODE_WIN32 )
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
namespace tcode { namespace io { namespace option {

#if defined( TCODE_WIN32 )
    typedef SOCKET native_handle;
#else
    typedef int native_handle;
    const int SOCKET_ERROR = -1;
#endif

    template < typename Type , int Level , int Opt >
    class sock_opt{
    public:
        sock_opt( void ){
        }
        
        sock_opt( const Type& t )
            : _opt(t){
        }
        
        bool set_option( native_handle fd )
        {
            return setsockopt( fd , Level , Opt , 
                    reinterpret_cast<char*>(&_opt) , sizeof( _opt )) != SOCKET_ERROR;
        }
        
        bool get_option( native_handle fd )
        {
            socklen_t len = sizeof(_opt);
            return getsockopt( fd , Level , Opt , 
                    reinterpret_cast<char*>(&_opt) , &len ) != SOCKET_ERROR;
        }
        
        Type& option( void ){
            return _opt;
        }
        void options( const Type& t ){
            _opt = t;
        }
    private:
        Type _opt;
    };

    
    class block{
    public:
        bool set_option( native_handle fd );
    };

    class nonblock{
    public:
        bool set_option( native_handle fd );      
    };
    
    typedef sock_opt< int       , SOL_SOCKET	, SO_REUSEADDR			> reuse_address;
    typedef sock_opt< int       , SOL_SOCKET	, SO_BROADCAST			> broad_cast;
    typedef sock_opt< int		, SOL_SOCKET	, SO_RCVBUF				> recv_buffer;
    typedef sock_opt< int		, SOL_SOCKET	, SO_RCVTIMEO			> recv_time;
    typedef sock_opt< int		, SOL_SOCKET	, SO_SNDBUF				> send_buffer;
    typedef sock_opt< int		, SOL_SOCKET	, SO_SNDTIMEO			> send_time;
    typedef sock_opt< int		, SOL_SOCKET	, SO_ERROR 			    > error; 
    typedef sock_opt< struct linger	, SOL_SOCKET	, SO_LINGER				> linger;
    
    class linger_remove_time_wait : public linger {
    public:
        linger_remove_time_wait( void );
    };

    typedef sock_opt< uint8_t	, IPPROTO_IP	, IP_MULTICAST_LOOP		> multi_cast_loop;
    typedef sock_opt< uint8_t	, IPPROTO_IP	, IP_MULTICAST_TTL		> multi_cast_ttl;
    typedef sock_opt< in_addr	, IPPROTO_IP	, IP_MULTICAST_IF		> multi_cast_if;
    typedef sock_opt< ip_mreq	, IPPROTO_IP	, IP_ADD_MEMBERSHIP		> multi_cast_join;
    typedef sock_opt< ip_mreq	, IPPROTO_IP	, IP_DROP_MEMBERSHIP	> multi_cast_drop;
            
            
    typedef sock_opt< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_LOOP	> multi_cast_loop_ipv6;
    typedef sock_opt< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_HOPS	> multi_cast_ttl_ipv6;
    typedef sock_opt< in6_addr	, IPPROTO_IPV6	, IPV6_MULTICAST_IF		> multi_cast_if_ipv6;
    typedef sock_opt< ipv6_mreq	, IPPROTO_IPV6	, IPV6_ADD_MEMBERSHIP	> multi_cast_join_ipv6;
    typedef sock_opt< ipv6_mreq	, IPPROTO_IPV6	, IPV6_DROP_MEMBERSHIP	> multi_cast_drop_ipv6;
    
    typedef sock_opt< int       , IPPROTO_TCP	, TCP_NODELAY		> tcp_no_delay;

#if defined( TCODE_WIN32 )
    typedef sock_opt< BOOL		, SOL_SOCKET	, SO_CONDITIONAL_ACCEPT		> conditional_accept;
    typedef sock_opt< int		, SOL_SOCKET	, SO_CONNECT_TIME			> connect_time;
    typedef sock_opt< SOCKET	, SOL_SOCKET	, SO_UPDATE_ACCEPT_CONTEXT	> update_accept_context;
#endif

}}}

#endif
