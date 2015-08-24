#ifndef __tcode_io_ip_h__
#define __tcode_io_ip_h__

#include <tcode/tcode.hpp>

#if defined( TCODE_WIN32 )


#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#endif

namespace tcode { namespace io { namespace ip {

    class handle {
    public:
#if defined( TCODE_WIN32 )
        typedef SOCKET native_type;
#else
        typedef int native_type;
#endif
        
        handle( void );
        explicit handle( native_type fd );
        
        handle( handle&& rhs );
        handle( const handle& rhs );

        handle operator=( native_type fd );
        handle operator=( const handle& rhs );
        handle operator=( handle&& rhs );

        ~handle( void );
        
        void close( void );

        template < typename opt >
        bool set_option( opt& o ){
            if ( good() )
                return o.set_option(native_handle());
            return false;    
        }
        template < typename opt >
        bool get_option( opt& o ){
            if ( good() )
                return o.get_option(native_handle());
            return false;
        }
        
        bool good( void ) const;
        native_type native_handle( void );

    private:
        native_type _handle;
    public:
        static native_type error;
        static native_type invalid;
    };

    
    
namespace option {
    template < typename Type , int Level , int Opt >
    class basic_opt{
    public:
        basic_opt( void ){
        }
        
        basic_opt( const Type& t )
            : _opt(t){
        }
        
        bool set_option( handle::native_type fd )
        {
            return setsockopt( fd , Level , Opt 
                    , reinterpret_cast<char*>(&_opt) 
                    , sizeof( _opt )) != handle::error;
        }
        
        bool get_option( handle::native_type fd )
        {
            socklen_t len = sizeof(_opt);
            return getsockopt( fd , Level , Opt  
                    , reinterpret_cast<char*>(&_opt) 
                    , &len ) != handle::error;
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
        bool set_option( int fd );
    };

    class nonblock{
    public:
        bool set_option( int fd );      
    };
    typedef basic_opt< int , SOL_SOCKET	, SO_REUSEADDR > reuse_address;
    typedef basic_opt< int , SOL_SOCKET	, SO_BROADCAST > broad_cast;
    typedef basic_opt< int , SOL_SOCKET	, SO_RCVBUF	   > recv_buffer;
    typedef basic_opt< int , SOL_SOCKET	, SO_RCVTIMEO  > recv_time;
    typedef basic_opt< int , SOL_SOCKET	, SO_SNDBUF	   > send_buffer;
    typedef basic_opt< int , SOL_SOCKET	, SO_SNDTIMEO  > send_time;
    typedef basic_opt< int , SOL_SOCKET	, SO_ERROR 	   > error; 
    typedef basic_opt< int , IPPROTO_TCP, TCP_NODELAY  > tcp_no_delay;
    typedef basic_opt< struct linger	, SOL_SOCKET	, SO_LINGER				> linger;
    
    class linger_remove_time_wait : public linger {
    public:
        linger_remove_time_wait( void );
    };

    typedef basic_opt< uint8_t	, IPPROTO_IP	, IP_MULTICAST_LOOP		> multi_cast_loop;
    typedef basic_opt< uint8_t	, IPPROTO_IP	, IP_MULTICAST_TTL		> multi_cast_ttl;
    typedef basic_opt< in_addr	, IPPROTO_IP	, IP_MULTICAST_IF		> multi_cast_if;
    typedef basic_opt< ip_mreq	, IPPROTO_IP	, IP_ADD_MEMBERSHIP		> multi_cast_join;
    typedef basic_opt< ip_mreq	, IPPROTO_IP	, IP_DROP_MEMBERSHIP	> multi_cast_drop;
            
    typedef basic_opt< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_LOOP	> multi_cast_loop_ipv6;
    typedef basic_opt< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_HOPS	> multi_cast_ttl_ipv6;
    typedef basic_opt< in6_addr	, IPPROTO_IPV6	, IPV6_MULTICAST_IF		> multi_cast_if_ipv6;
#if !defined( TCODE_APPLE )
    typedef basic_opt< ipv6_mreq	, IPPROTO_IPV6	, IPV6_ADD_MEMBERSHIP	> multi_cast_join_ipv6;
    typedef basic_opt< ipv6_mreq	, IPPROTO_IPV6	, IPV6_DROP_MEMBERSHIP	> multi_cast_drop_ipv6;
#endif 

#if defined( TCODE_WIN32 )
    typedef basic_opt< BOOL		, SOL_SOCKET	, SO_CONDITIONAL_ACCEPT		> conditional_accept;
    typedef basic_opt< int		, SOL_SOCKET	, SO_CONNECT_TIME			> connect_time;
    typedef basic_opt< SOCKET	, SOL_SOCKET	, SO_UPDATE_ACCEPT_CONTEXT	> update_accept_context;
#endif

}

}}}

#endif
