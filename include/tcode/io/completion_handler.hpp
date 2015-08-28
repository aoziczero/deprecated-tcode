#ifndef __tcode_io_completion_handler_h__
#define __tcode_io_completion_handler_h__

#include <tcode/slist.hpp>

namespace tcode { namespace io {

template < typename Handler >
class completion_handler 
    : public tcode::slist::node< Handler >     
{
public:
    typedef void (*call_function)( void* handle 
            , int ev 
            , completion_handler* handler );

    completion_handler( call_function fn)
        : _call_function( fn )
    {
    }
    void operator()( void* handle , int ev ) {
        _call_function( handle , ev , this );
    }
protected:
    ~completion_handler( void ){
    }
private:
    call_function _call_function;
};

}}


#endif
