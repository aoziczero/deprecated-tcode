#ifndef __tcode_io_epoll_h__
#define __tcode_io_epoll_h__

#include <tcode/io/io.hpp>
#include <tcode/io/pipe.hpp>
#include <tcode/function.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/operation.hpp>
#include <tcode/threading/spinlock.hpp>

namespace tcode { namespace io {
namespace ip {

    namespace tcp {
        class reactor_completion_handler_connect_base;
    }
   
    namespace udp {
    }
}

    class epoll {
    public:
        struct _descriptor;
        typedef _descriptor* descriptor;
        typedef int native_descriptor;

        epoll( void );
        ~epoll( void );

        int run( const tcode::timespan& ts );

        void wake_up( void );

        bool bind( int fd , descriptor& d );
        void unbind( descriptor& d );

        void execute( tcode::operation* op ); 

        void connect( descriptor& d 
                , ip::tcp::reactor_completion_handler_connect_base* op );
    private:
        int _handle;
        tcode::io::pipe _wake_up;
        tcode::threading::spinlock _lock;
        tcode::slist::queue< tcode::operation > _op_queue; 
    };

    /*
    class epoll {
    public:
        epoll( void );
        ~epoll( void );

        bool bind( int fd , int ev , tcode::io::event_handler* handler );
        void unbind( int fd );
        int run( const tcode::timespan& ts );
        void wake_up( void );
        void wake_up_handler( int ev );
    private:
        int _handle;
        io::pipe _pipe;
        tcode::function< void (int)> _pipe_handler;
    };
    */
    
    
    
}}

#endif
