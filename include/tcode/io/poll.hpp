#ifndef __tcode_io_poll_h__
#define __tcode_io_poll_h__

#include <tcode/io/io.hpp>
#include <tcode/threading/spinlock.hpp>
#include <tcode/function.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/io/pipe.hpp>

#include <sys/poll.h>

namespace tcode { namespace io {

    class poll {
    public:
        poll( void );
        ~poll( void );

        bool bind( int fd , int ev , tcode::function< void ( int ) >* handler );
        void unbind( int fd );
        int run( const tcode::timespan& ts );
        void wake_up( void );
        void wake_up_handler( int ev );
    private:
        std::vector< struct pollfd > _fds;
        std::vector< tcode::function< void ( int ) >* > _handler;
        io::pipe _pipe;
        tcode::function< void (int)> _pipe_handler;
    };

}}

#endif
