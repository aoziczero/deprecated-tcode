#ifndef __tcode_io_ip_address_h__
#define __tcode_io_ip_address_h__

#if defined (TCODE_WIN32)

#else
#include <netinet/in.h>
#endif

namespace tcode { namespace io { namespace ip {

    /*!
     * @class address
     * @brief 소켓 주소용도로 작업중인데
     * uds 도 사용가능하도록 처리해볼것
     */
    class address{
    public:
        class any;

        address( void );
        address( const any& any );
        address( struct sockaddr* addr , int len );

        address( const char* ip , const uint16_t port );
        ~address( void );

        struct sockaddr* sockaddr( void ) const;

        std::string ip( void );
        int		    port( void ) const;
    	int			family( void ) const;

        /*!
         * @class any
         * @brief inaddr_any
         */
        class any {
        public:
            any( int f , int p );
            int port;
            int family;
        };
    private:
        struct sockaddr_storage _address;
        socklen_t _len;
    };

}}}

#endif
