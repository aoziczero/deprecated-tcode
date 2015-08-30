#ifndef __tcode_io_completion_handler_h__
#define __tcode_io_completion_handler_h__

#include <tcode/slist.hpp>

namespace tcode { namespace io {

template < typename Handler >
class completion_handler 
    : public tcode::slist::node< Handler >        
{
public:
    typedef void (*call_function)( completion_handler* 
            , void* );

    completion_handler( call_function fn)
        : _call_function( fn )
    {
    }

    void complete( void* handle ) {
        _call_function( this , handle );
    }
protected:
    ~completion_handler( void ){
    }
private:
    call_function _call_function;
};

}}


#endif
