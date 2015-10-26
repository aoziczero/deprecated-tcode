#ifndef __tcode_io_ip_raw_h__
#define __tcode_io_ip_raw_h__

#include <tcode/tcode.hpp>
#include <tcode/io/engine.hpp>
#include <tcode/io/ip/udp/operation_write.hpp>
#include <tcode/io/ip/udp/operation_read.hpp>

namespace tcode { namespace io { namespace ip { namespace raw {

    TCODE_PACK(
    struct ether_header{
        uint8_t ether_dhost[6];
        uint8_t ether_shost[6];
        uint16_t ether_type; 
    });

    TCODE_PACK(
    struct iphdr{
/*
#if defined TCODE_BIG_ENDIAN
        unsigned int version:4;
        unsigned int ihl:4;
#else
        unsigned int ihl:4;
        unsigned int version:4;
#endif
*/
		unsigned int ihl : 4;
		unsigned int version : 4;
        uint8_t tos;
        uint16_t tot_len;
        uint16_t id;
        uint16_t frag_off;
        uint8_t ttl;
        uint8_t protocol;
        uint16_t check;
        uint32_t saddr;
        uint32_t daddr;
    });

    TCODE_PACK(
    struct arphdr{
        uint16_t ar_hrd;
        uint16_t ar_pro;
        uint8_t ar_hln;
        uint8_t ar_pln;
        uint16_t ar_op;
        uint8_t ar_sha[6];
        uint8_t ar_sip[4];
        uint8_t ar_tha[6];
        uint8_t ar_tip[4];
    });


    /*!
     * @class socket
     * @brief 
     */
    class socket {
    public:
        //! ctor
        socket( io::engine& e );

        //! move ctor
        socket( socket&& rhs );

        //! dtor
        ~socket( void );


        int write( const tcode::io::buffer& buf 
                , const tcode::io::ip::address& addr );
        
        int read( tcode::io::buffer& buf 
                , tcode::io::ip::address& addr );
        
        //!
        template < typename Handler >
        void write( const tcode::io::buffer& buf 
                , const tcode::io::ip::address& addr 
                , const Handler& handler )
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( io::ip::udp::operation_write<Handler> ));
            new (ptr) io::ip::udp::operation_write<Handler>( buf , addr , handler );
            engine().mux().write( _descriptor
                    , reinterpret_cast< io::ip::udp::operation_write_base* >(ptr));
        }
       
        //!
        template < typename Handler >
        void read( const tcode::io::buffer& buf 
                , const Handler& handler ) 
        {
            void* ptr = tcode::operation::alloc( 
                    sizeof( io::ip::udp::operation_read<Handler> ));
            new (ptr) io::ip::udp::operation_read<Handler>( buf , handler );
            engine().mux().read( _descriptor
                    , reinterpret_cast< io::ip::udp::operation_read_base* >(ptr));
        }
       
        bool open( int af , int type , int proto );
        void close( void );
        io::descriptor& descriptor( void );
        io::engine& engine(void);
    private:
        io::engine* _engine;
        io::descriptor _descriptor;
    private:
        socket( void );
        socket( const socket& rhs );
        socket operator=( const socket& rhs );
        socket operator=( socket&& rhs );
    };

}}}}

#endif
