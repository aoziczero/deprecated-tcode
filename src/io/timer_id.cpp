#include "stdafx.h"
#include <tcode/io/timer_id.hpp>

namespace tcode { namespace io {

    timer::id::id( io::engine& e )
        : engine(e)
    {
        refcount.store(0);
    }

    timer::id::~id( void ){
    }

    void timer::id::add_ref( void ){
        refcount.fetch_add(1);
    }

    void timer::id::release( void ) {
        if( refcount.fetch_sub(1,std::memory_order_release) == 1){
            delete this;
        }
    }

}}
