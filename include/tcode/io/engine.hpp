#ifndef __tcode_io_engine_h__
#define __tcode_io_engine_h__

#include <tcode/tcode.hpp>
#include <tcode/active_ref.hpp>
#include <tcode/io/io.hpp>
#include <tcode/io/io_define.hpp>
#include <tcode/threading/spinlock.hpp>
#include <tcode/io/timer.hpp>

#include <thread>
#include <list>

namespace tcode { namespace io { 

    /*!
     *  @class  engine
     *  @brief  
     */
    class engine {
    public:
		//! ctor
        engine( void );
		//! dtor
        ~engine( void );

		//! run
        void run( void );

		//! check current context is in run loop
        bool in_run_loop( void );

        void timer_schedule( timer::id* id );
        void timer_cancel( timer::id* id );
        void timer_drain( void );
        tcode::timespan next_wake_up_time( void );

        io::multiplexer& mux( void );

        void active_inc( void );
        void active_dec( void );

        template< typename Handler >
        void execute( const Handler& handler ){
            mux().execute( tcode::operation::wrap( handler ));
        }
    private:
        io::multiplexer _mux;
        std::atomic<int> _active;
        std::thread::id _run_thread_id;
        std::list< timer::id* > _timers;
    };

}}

#endif
