#ifndef __tcode_io_engine_h__
#define __tcode_io_engine_h__

#include <tcode/tcode.hpp>
#include <tcode/threading/spinlock.hpp>
#include <tcode/io/timer.hpp>

#include <thread>
#include <atomic>
#include <list>

#if defined ( TCODE_WIN32 )
#elif defined( TCODE_LINUX )
#include <tcode/io/epoll.hpp>
#elif defined( TCODE_APPLE )
#else

#endif
namespace tcode { namespace io { 

    /*!
     *  @class  engine
     *  @brief  
     */
    class engine {
    public:
#if defined ( TCODE_WIN32 )
        typedef io::completion_port impl;
#elif defined( TCODE_LINUX )
        typedef io::epoll impl;
#elif defined( TCODE_APPLE )
        typedef io::kqueue impl;
#else

#endif
    public:
        engine( void );
        ~engine( void );

        typedef tcode::function<void () > operation;

        void post( const operation& op );

        void run( void );

        bool in_run_loop( void );

        bool bind( int fd , int ev , tcode::io::event_handler* handler);
        void unbind( int fd );

        void active_inc( void );
        void active_dec( void );

        void timer_schedule( timer::id* id );
        void timer_cancel( timer::id* id );
        void timer_drain( void );
        tcode::timespan next_wake_up_time( void );
    private:
        impl _impl;
        std::thread::id _run_thread_id;
        std::atomic<int> _active;
        class op_queue {
        public:
            op_queue( void );
            ~op_queue( void );
            void post( const operation& op );
            int drain( void );
        private:
            tcode::threading::spinlock _lock;
            std::vector< operation > _ops;
            std::vector< operation > _swap;
        };
        op_queue _op_queue;
        std::list< timer::id* > _timers;
    };

}}

#endif
