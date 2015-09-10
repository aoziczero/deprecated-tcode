#ifndef __tcode_io_ip_tcp_pipeline_filter_h__
#define __tcode_io_ip_tcp_pipeline_filter_h__

#include <vector>

namespace tcode { namespace io { namespace ip { namespace tcp { namespace pipeline {

    class filter {
    public:
       filter( void );
       ~filter( void );
    private:
        std::vector< filter* > _filters;        
    };

}}}}}

#endif
