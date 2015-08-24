#ifndef __tcode_io_ip_optino_h__
#define __tcode_io_ip_optino_h__

#include <tcode/io/ip/ip.hpp>

namespace tcode { namespace io { namespace ip { namespace option {

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
        bool set_option( socket_type fd );
    };

    class non_blocking {
    public:
        bool set_option( socket_type fd );
    };
    
    typedef value_option< int , SOL_SOCKET	, SO_REUSEADDR  > reuse_address;
    typedef value_option< int , SOL_SOCKET	, SO_BROADCAST  > broad_cast;
    typedef value_option< int , SOL_SOCKET	, SO_RCVBUF	    > recv_buffer;
    typedef value_option< int , SOL_SOCKET	, SO_RCVTIMEO   > recv_time;
    typedef value_option< int , SOL_SOCKET	, SO_SNDBUF	    > send_buffer;
    typedef value_option< int , SOL_SOCKET	, SO_SNDTIMEO   > send_time;
    typedef value_option< int , SOL_SOCKET	, SO_ERROR 	    > error; 
    typedef value_option< struct linger	, SOL_SOCKET	, SO_LINGER > linger;
        
    class linger_remove_time_wait : public linger {
    public:
        linger_remove_time_wait( void );
    };

    typedef value_option< uint8_t	, IPPROTO_IP	, IP_MULTICAST_LOOP		> multi_cast_loop;
    typedef value_option< uint8_t	, IPPROTO_IP	, IP_MULTICAST_TTL		> multi_cast_ttl;
    typedef value_option< in_addr	, IPPROTO_IP	, IP_MULTICAST_IF		> multi_cast_if;
    typedef value_option< ip_mreq	, IPPROTO_IP	, IP_ADD_MEMBERSHIP		> multi_cast_join;
    typedef value_option< ip_mreq	, IPPROTO_IP	, IP_DROP_MEMBERSHIP	> multi_cast_drop;


    typedef value_option< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_LOOP	> multi_cast_loop_ipv6;
    typedef value_option< uint8_t	, IPPROTO_IPV6	, IPV6_MULTICAST_HOPS	> multi_cast_ttl_ipv6;
    typedef value_option< in6_addr	, IPPROTO_IPV6	, IPV6_MULTICAST_IF		> multi_cast_if_ipv6;
#if !defined( TCODE_APPLE )
    typedef value_option< ipv6_mreq	, IPPROTO_IPV6	, IPV6_ADD_MEMBERSHIP	> multi_cast_join_ipv6;
    typedef value_option< ipv6_mreq	, IPPROTO_IPV6	, IPV6_DROP_MEMBERSHIP	> multi_cast_drop_ipv6;
#endif   
    typedef value_option< int       , IPPROTO_TCP	, TCP_NODELAY		> tcp_no_delay;

#if defined( TCODE_WIN32) 
    typedef value_option< BOOL		, SOL_SOCKET	, SO_CONDITIONAL_ACCEPT		> conditional_accept;
    typedef value_option< int		, SOL_SOCKET	, SO_CONNECT_TIME			> connect_time;
    typedef value_option< SOCKET	, SOL_SOCKET	, SO_UPDATE_ACCEPT_CONTEXT	> update_accept_context;
#endif



}}}}

#endif
