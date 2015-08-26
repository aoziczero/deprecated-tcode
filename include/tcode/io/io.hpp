#ifndef __tcode_io_h__
#define __tcode_io_h__

#include <tcode/function.hpp>

namespace tcode { namespace io {

    enum {
        EV_READ = 0x01, 
        EV_WRITE = 0x02,
    };

    typedef tcode::function< void (int) > event_handler;

}}

#endif
