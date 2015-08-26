#ifndef __tcode_io_epoll_h__
#define __tcode_io_epoll_h__

#include <tcode/io/io.hpp>
#include <tcode/io/pipe.hpp>
#include <tcode/function.hpp>
#include <tcode/time/timespan.hpp>


namespace tcode { namespace io {

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

}}

#endif
