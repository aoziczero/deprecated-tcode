#ifndef __tcode_reactor_completion_handler_h__
#define __tcode_reactor_completion_handler_h__

#include <tcode/io/completion_handler.hpp>

namespace tcode { namespace io {

    class reactor_completion_handler
        : public completion_handler< reactor_completion_handler >
    {
    public:
        typedef typename completion_handler<
            reactor_completion_handler
        >::call_function call_function;

        typedef bool (*io_function)( reactor_completion_handler*  );

        reactor_completion_handler( call_function fn ,
                io_function iofn );
        ~reactor_completion_handler( void );

        bool operator()( void );
    private:
        io_function _io_function; 
    };
}}

#endif
