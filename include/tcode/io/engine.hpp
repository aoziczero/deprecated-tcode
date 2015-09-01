#ifndef __tcode_io_engine_h__
#define __tcode_io_engine_h__

#include <tcode/tcode.hpp>
#include <tcode/active_ref.hpp>
#include <tcode/threading/spinlock.hpp>
#include <tcode/io/timer.hpp>

#include <thread>
#include <list>

#if defined ( TCODE_WIN32 )
#include <tcode/io/completion_port.hpp>
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
        typedef io::completion_port impl_type;
#elif defined( TCODE_LINUX )
        typedef io::epoll impl_type;
#elif defined( TCODE_APPLE )
        typedef io::kqueue impl_type;
#else

#endif
        typedef impl_type::descriptor descriptor;
    public:
        engine( void );
        ~engine( void );

        void run( void );

        bool in_run_loop( void );

        void timer_schedule( timer::id* id );
        void timer_cancel( timer::id* id );
        void timer_drain( void );
        tcode::timespan next_wake_up_time( void );

        impl_type& impl( void );

        tcode::active_ref& active( void );

        template< typename Handler >
        void execute( const Handler& handler ){
            impl().execute( tcode::operation::wrap( handler ));
        }
    private:
        impl_type _impl;
        tcode::active_ref _active;
        std::thread::id _run_thread_id;
        std::list< timer::id* > _timers;
    };

}}

#endif
