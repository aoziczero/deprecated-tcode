#ifndef __tcode_io_h__
#define __tcode_io_h__

#include <tcode/tcode.hpp>

#include <tcode/function.hpp>

namespace tcode { namespace io {

    enum {
        EV_READ = 0x01, 
        EV_WRITE = 0x02,
    };

    enum { ev_read = 0 , ev_write = 1 , ev_pri = 2 , ev_accept = 0 , ev_connect = 1 , ev_max = 3 };

    typedef tcode::function< void (int) > event_handler;

}}

#endif
