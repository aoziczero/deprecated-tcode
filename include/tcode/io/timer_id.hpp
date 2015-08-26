#ifndef __tcode_io_timer_id_h__
#define __tcode_io_timer_id_h__

#include <tcode/io/timer.hpp>

namespace tcode { namespace io {

    struct timer::id {
        id ( io::engine& e );
        ~id( void );
        
        void add_ref( void );
        void release( void );
        

        io::engine& engine;
        std::atomic<int> refcount;
        tcode::timestamp due_time;
        tcode::timespan repeat;
        tcode::function< void ( const std::error_code& ec ) > callback;
    };

}}

#endif
