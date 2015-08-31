#include "stdafx.h"
#include <tcode/io/timer.hpp>
#include <tcode/io/timer_id.hpp>
#include <tcode/io/engine.hpp>

namespace tcode { namespace io {

    timer::timer( io::engine& e ) 
        : _id ( new timer::id( e ))
    {
        _id->add_ref();
    }

    timer::~timer( void ) {
        _id->release();
    }

    timer& timer::due_time( const tcode::timespan& ts ) {
        return due_time( tcode::timestamp::now() + ts ); 
    }
    
    timer& timer::due_time( const tcode::timestamp& ts ) {
        _id->due_time = ts;
        return *this;
    }

    timer& timer::repeat( const tcode::timespan& ts ){
        _id->repeat = ts;
        return *this;
    }

    timer& timer::callback( const tcode::function< void ( const std::error_code& ) >& cb ){
        _id->callback = cb;
        return *this;
    }

    void timer::fire( void ) {
        _id->engine.timer_schedule( _id ); 
    }

    void timer::cancel( void ) {
        if( _id->refcount.load() != 1 ) 
            _id->engine.timer_cancel(_id);
    }

}}
