#ifndef __tcode_io_ip_tcp_channel_h__
#define __tcode_io_ip_tcp_channel_h__

#include <tcode/byte_buffer.hpp>
#include <tcode/io/ip/tcp/pipeline/pipeline.hpp>
#include <tcode/io/ip/tcp/socket.hpp>
#include <atomic>

namespace tcode{ namespace io { namespace ip { namespace tcp{ 

    class pipeline;
    
    class channel{
    public:
        channel( tcode::io::ip::tcp::socket&& fd );
        ~channel( void );

        tcode::io::engine& engine( void );
        tcp::pipeline& pipeline( void );

        void close( void );
        void close( const std::error_code& ec );
        
        void add_ref( void );
        int release( void );
        
        void fire_on_open( const tcode::io::ip::address& addr );
        void fire_on_end_reference( void );
        void fire_on_close( const std::error_code& ec );

        void do_write( tcode::byte_buffer buf );

        void read( void );
        void handle_read( const std::error_code& ec 
                , const int nbyte );
        void handle_write( const std::error_code& ec 
                , const int nbyte);
        void write_remains( void  );
    private:
        tcode::io::ip::tcp::socket _fd;
        tcode::io::ip::tcp::pipeline _pipeline;
        std::atomic< int > _flag;
        std::vector< tcode::byte_buffer > _write_buffers;
        tcode::byte_buffer _buffer;
    };

}}}}


#endif
