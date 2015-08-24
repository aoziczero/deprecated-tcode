#ifndef __tcode_io_ip_simple_select_h__
#define __tcode_io_ip_simple_select_h__

#include <tcode/io/ip/ip.hpp>
#include <tcode/time/timespan.hpp>

namespace tcode { namespace io { namespace ip {

    class selector{
    public:
        static bool wait_for_read( socket_type fd , const tcode::timespan& wait );
        static bool wait_for_write( socket_type fd , const tcode::timespan& wait );
    };

}}}

#endif
