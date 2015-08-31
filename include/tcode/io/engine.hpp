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
        typedef io::completion_port impl_type;
#elif defined( TCODE_LINUX )
        typedef io::epoll impl_type;
#elif defined( TCODE_APPLE )
        typedef io::kqueue impl_type;
#else

#endif
        typedef impl_type::descriptor descriptor;
        typedef impl_type::native_descriptor native_descriptor;
    public:
        engine( void );
        ~engine( void );

        void run( void );

        bool in_run_loop( void );

        void active_inc( void );
        void active_dec( void );

        void timer_schedule( timer::id* id );
        void timer_cancel( timer::id* id );
        void timer_drain( void );
        tcode::timespan next_wake_up_time( void );

        impl_type& impl( void );
    private:
        template < typename Handler >
        class op : public tcode::operation{
        public:
            op( engine& e , const Handler& h )
                : operation( op::exec )
                , _engine(e)
                , _handler( h )
            { _engine.active_inc(); }
            ~op( void ) {  _engine.active_dec(); }
            static void exec( operation* op_base ) {
                op* pop( static_cast< op* >(op_base));
                Handler h( std::move( pop->_handler ));
                pop->~op();
                operation::free( pop );
                h();
            }
        private:
            engine& _engine;
            Handler _handler;
        };
    public:
        template< typename Handler >
        void execute( const Handler& handler ){
            void* ptr = tcode::operation::alloc(sizeof( op<Handler>));
            new (ptr) engine::op<Handler>( *this , handler );
            impl().execute( reinterpret_cast<tcode::operation*>(ptr) );
        }
    private:
        impl_type _impl;
        std::thread::id _run_thread_id;
        std::atomic<int> _active;
        std::list< timer::id* > _timers;
    };

}}

#endif
