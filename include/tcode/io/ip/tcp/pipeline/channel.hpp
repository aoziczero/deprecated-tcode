#ifndef __tcode_io_ip_tcp_channel_h__
#define __tcode_io_ip_tcp_channel_h__

#include <tcode/byte_buffer.hpp>
//#include <tcode/io/ip/tcp/pipeline/pipeline.hpp>
//#include <tcode/io/ip/tcp/pipeline/packet_buffer.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <atomic>

namespace tcode{ namespace io { 

    class engine; 

namespace ip { namespace tcp {

    class pipeline;

    class channel{
    public:
        channel( tcp::socket&& fd );
        ~channel( void );

        io::engine& engine( void );
        tcp::pipeline& pipeline( void );

        void close( void );
        void close( const std::error_code& ec );
        
        void add_ref( void );
        int release( void );
        
        void fire_on_open( const tcode::io::ip::address& addr );
        void fire_on_end_reference( void );
        void fire_on_close( const std::error_code& ec );

        void do_write( tcode::buffer::byte_buffer buf );

//        void packet_buffer( packet_buffer_ptr& ptr );
        void read( void );
        void handle_read( const std::error_code& ec 
                , const int nbyte );
        void handle_write( const std::error_code& ec 
                , const int nbyte);
        void write_remains( void  );
    private:
        tcp::socket _fd;
//        tcp::pipeline _pipeline;
        std::atomic< int > _flag;
        std::vector< tcode::byte_buffer > _write_buffers;
 //       packet_buffer_ptr _packet_buffer;
    };

}}}}


#endif
