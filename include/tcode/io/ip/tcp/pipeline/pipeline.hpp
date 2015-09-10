#ifndef __tcode_io_ip_tcp_pipeline_h__
#define __tcode_io_ip_tcp_pipeline_h__

#include <tcode/io/ip/tcp/pipeline/channel.hpp>
#include <tcode/io/ip/tcp/pipeline/filter.hpp>
#include <tcode/io/ip/tcp/pipeline/builder.hpp>
#include <tcode/io/ip/tcp/pipeline/connector.hpp>
#include <tcode/io/ip/tcp/pipeline/acceptor.hpp>

namespace tcode { namespace io { namespace ip { namespace tcp { namespace pipeline {

    class pipeline {
    public:
    private:
        std::vector< filter* > _filters; 
    };

}}}}}

#endif
