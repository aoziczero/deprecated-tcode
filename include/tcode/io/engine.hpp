#ifndef __tcode_io_engine_h__
#define __tcode_io_engine_h__

#include <tcode/tcode.hpp>
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

        //! add timer
        void timer_schedule( timer::id* id );
        //! timer cancel
        //! if timers contain id call id::callback
        void timer_cancel( timer::id* id );
        //! drain timers
        void timer_drain( void );
        //!
        tcode::timespan next_wake_up_time( void );

        io::multiplexer& mux( void );

        void add_ref( void );
        void release( void );

        template< typename Handler >
        void execute( const Handler& handler ){
            mux().execute( tcode::operation::wrap( handler ));
        }
    private:
        io::multiplexer _mux;
        std::atomic<int> _refcount;
        std::thread::id _run_thread_id;
        std::list< timer::id* > _timers;
    };

}}

#endif
