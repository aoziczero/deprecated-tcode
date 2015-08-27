#ifndef __tcode_io_completion_handler_h__
#define __tcode_io_completion_handler_h__

namespace tcode { namespace io {

class completion_handler {
public:
    typedef void (*call_function)( void* handle 
            , int ev 
            , completion_handler* handler );

    completion_handler( call_function fn);  
    ~completion_handler( void );
    void operator()( void* handle , int ev );
    completion_handler*& next(void);
private:
    call_function _call_function;
    completion_handler* _next;
};

}}


#endif
