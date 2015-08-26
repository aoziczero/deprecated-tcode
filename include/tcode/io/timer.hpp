#ifndef __tcode_io_timer_h__
#define __tcode_io_timer_h__

#include <tcode/time/timespan.hpp>
#include <tcode/time/timestamp.hpp>
#include <tcode/function.hpp>
#include <atomic>

namespace tcode { namespace io {
/*
    extern std::atomic<int> id_alloc;
    extern std::atomic<int> id_free;
    extern std::atomic<int> id_add_ref;
    extern std::atomic<int> id_release;
*/
    class engine;
    
    class timer {
    public:
        struct id;
        timer( io::engine& e );
        ~timer( void );

        timer& due_time( const tcode::timespan& ts );
        timer& due_time( const tcode::timestamp& ts );

        timer& repeat( const tcode::timespan& ts );
        timer& callback( const tcode::function<void (const std::error_code& ) >& cb );

        void fire( void );
        void cancel( void );
    private:
        timer::id* _id;
    };

    int test_alive_ids( void );

}}

#endif
