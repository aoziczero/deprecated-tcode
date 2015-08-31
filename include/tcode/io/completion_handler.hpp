#ifndef __tcode_io_completion_handler_h__
#define __tcode_io_completion_handler_h__

#include <tcode/slist.hpp>

namespace tcode { namespace io {

template < typename Handler >
class completion_handler 
    : public tcode::slist::node< Handler >        
{
public:
    typedef void (*completion_function)( completion_handler* 
            , void* );

    completion_handler( completion_function fn)
        : _completion_function( fn )
    {
    }

    void complete( void* handle ) {
        _completion_function( this , handle );
    }
protected:
    ~completion_handler( void ){
    }
private:
    completion_function _completion_function;
};

void* completion_handler_alloc( int sz );
void  completion_handler_free( void* p );

}}


#endif
