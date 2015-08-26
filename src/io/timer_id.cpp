#include "stdafx.h"
#include <tcode/io/timer_id.hpp>

namespace tcode { namespace io {
    std::atomic<int> alive;
    int test_alive_ids( void ) {
        return alive.load();
    }
   /* 
    std::atomic<int> id_alloc;
    std::atomic<int> id_free;
    std::atomic<int> id_add_ref;
    std::atomic<int> id_release;
    */

    timer::id::id( io::engine& e )
        : engine(e)
    {
        refcount.store(0);
        alive.fetch_add(1);
    }

    timer::id::~id( void ){
    }

    void timer::id::add_ref( void ){
        refcount.fetch_add(1);
        //id_add_ref.fetch_add(1);
    }

    void timer::id::release( void ) {
        //id_release.fetch_add(1);
        if( refcount.fetch_sub(1,std::memory_order_release) == 1){
            delete this;
            alive.fetch_sub(1);
            //id_free.fetch_add(1);
        }
    }

}}
