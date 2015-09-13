#ifndef __tcode_io_uds_address_h__
#define __tcode_io_uds_address_h__


#include <arpa/inet.h>
#include <sys/un.h>

namespace tcode { namespace io { namespace uds {

    /*!
     * @class address
     * @brief unix domain socket address
     */
    class address{
    public:
        //! 기본 생성자
        address( void );

        //! native_type 에서 생성
        address( const struct sockaddr_un& addr );

        address( const address& rhs );

        address( const char* file );
        
        ~address( void );

        struct sockaddr* sockaddr( void ) const;
        socklen_t        sockaddr_length( void ) const;
        socklen_t*       sockaddr_length_ptr( void );


        std::string file( void ) const;
    private:
        struct sockaddr_un _address;
        socklen_t _len;
    };

}}}

#endif
