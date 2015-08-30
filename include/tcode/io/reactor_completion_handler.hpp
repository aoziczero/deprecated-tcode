#ifndef __tcode_reactor_completion_handler_h__
#define __tcode_reactor_completion_handler_h__

#include <tcode/io/completion_handler.hpp>

namespace tcode { namespace io {

    class reactor_completion_handler
        : public completion_handler< reactor_completion_handler >
    {
    public:
        typedef completion_handler< reactor_completion_handler > base_type;

        typedef bool (*io_function)( reactor_completion_handler* , void* );

        reactor_completion_handler( base_type::call_function fn ,
                io_function iofn );
        ~reactor_completion_handler( void );

        bool do_reactor_io( void* handle );
    private:
        io_function _io_function; 
    };
}}

#endif
