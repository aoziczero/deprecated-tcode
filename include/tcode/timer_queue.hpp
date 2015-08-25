#ifndef __tcode_timer_queue_h__
#define __tcode_timer_queue_h__

#include <tcode/time/timestamp.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/threading/spinlock.hpp>

#include <atomic>

#include <list>

namespace tcode {


    class timer_queue {
    public:
        class timer {
        public:
            timer( void );

            timer( const tcode::timestamp& expired_at 
                  ,const tcode::function< void () >& on_timer);

            timer( const timer& rhs );

            ~timer( void );

            void start( void );
            void stop( void );

            
        private:
            tcode::timestamp _expired_at;
            tcode::function< void ( const std::error_code& ) > _on_timer;
        };

    public:
        timer_queue( void );
        ~timer_queue( void );

        void start( const timer_ptr& timer );
        void stop( const timer_ptr& timer );

        void 
    private:
        std::list< timer_ptr > _timers;
    };


}

#endif
