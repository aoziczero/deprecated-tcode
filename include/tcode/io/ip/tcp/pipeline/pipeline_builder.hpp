#ifndef __tcode_io_ip_tcp_pipeline_builder_h__
#define __tcode_io_ip_tcp_pipeline_builder_h__

#include <system_error>

namespace tcode { namespace io {

    class engine;

    namespace ip { namespace tcp {

        class pipeline;
        class pipeline_builder 
        {
        public:
            pipeline_builder( void ){}
            virtual ~pipeline_builder( void ){}
            virtual io::engine& engine( void ) = 0;
            virtual bool build( pipeline& p ){
                return false;
            }
        };

}}}}

#endif
