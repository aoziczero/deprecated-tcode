#ifndef __tcode_io_ip_tcp_pipeline_filter_h__
#define __tcode_io_ip_tcp_pipeline_filter_h__

#include <vector>

namespace tcode { namespace io { namespace ip { namespace tcp { namespace pipeline {

    class filter {
    public:
        filter( void );
        virtual ~filter( void );

        virtual void on_open( const tcode::io::ip::address& addr );
        virtual void on_close( void );
        virtual void on_error( const std::error_code& e );
        virtual void on_read( tcode::byte_buffer& buf );



    private:
    };

}}}}}

#endif
