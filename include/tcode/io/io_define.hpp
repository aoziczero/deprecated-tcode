#ifndef __tcode_io_define_h__
#define __tcode_io_define_h__

#if defined ( TCODE_WIN32 )
#include <tcode/io/completion_port.hpp>
#elif defined( TCODE_LINUX )
#include <tcode/io/epoll.hpp>
#include <tcode/io/poll.hpp>
#elif defined( TCODE_APPLE )
#include <tcode/io/kqueue.hpp>
#else

#endif

namespace tcode { namespace io {

#if defined ( TCODE_WIN32 )
    typedef completion_port multiplexer;
#elif defined( TCODE_LINUX )
#if defined (MUX_POLL)
    typedef poll multiplexer;
#else
    typedef epoll multiplexer;
#endif
#elif defined( TCODE_APPLE )
#if defined (MUX_POLL)
    typedef poll multiplexer;
#else
    typedef kqueue multiplexer;
#endif
#else
#endif
    typedef multiplexer::descriptor descriptor;
}}


#endif
