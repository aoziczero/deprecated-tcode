#ifndef __tcode_io_define_h__
#define __tcode_io_define_h__

#include <tcode/io/select.hpp>
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

#if defined( MUX_POLL )
    typedef io::poll multiplexer;
#elif defined( MUX_SELECT )
    typedef io::select multiplexer;
#else
#if defined ( TCODE_WIN32 )
    typedef completion_port multiplexer;
#elif defined( TCODE_LINUX )
    typedef epoll multiplexer;
#elif defined( TCODE_APPLE )
    typedef kqueue multiplexer;
#endif
#endif
    typedef multiplexer::descriptor descriptor;
}}


#endif
