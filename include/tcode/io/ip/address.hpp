#ifndef __tcode_io_ip_address_h__
#define __tcode_io_ip_address_h__

#if defined (TCODE_WIN32)

#else
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

namespace tcode { namespace io { namespace ip {

    /*!
     * @class address
     * @brief 소켓 주소 ipv4 ipv6
     */
    class address{
    public:
        class any;
        class dns;
        
        //! 기본 생성자
        address( void );

        //! server bind 용 address
        address( const any& any );

        //! native_type 에서 생성
        address( struct sockaddr* addr , int len );

        //! ip 문자열에서 주소를 가져오는 생성자
        //! ip v4
        address( const char* ip , const uint16_t port );
        
        //! ip / dns 로 부터 주소를 가져오는 생성자
        address( const int af , const dns& dns , const uint16_t port );
        
        ~address( void );

        struct sockaddr* sockaddr( void ) const;
        socklen_t        sockaddr_length( void ) const;
        socklen_t*       sockaddr_length_ptr( void );


        std::string ip( void ) const;
        int		    port( void ) const;
    	int			family( void ) const;

        /*!
         * @class any
         * @brief bind 용 주소 생성용 파라미터 
         */
        class any {
        public:
            any( int p );
            int port;
        };

        /*!
         * @class dns
         * @brief
         */
        class dns{
        public:
            dns( const char* d );
            const char* domain;
        };
        
        //! dns 로 부터 address 목록 가져오기
        //! @param af address family AF_UNSPEC AF_INET AF_INET6 등등
        static std::vector<address> from_dns( const int af
                , const dns& dns 
                , const uint16_t port 
                , const int type = SOCK_STREAM
                , const int flags = AI_PASSIVE );
    private:
        struct sockaddr_storage _address;
        socklen_t _len;
    };

}}}

#endif
