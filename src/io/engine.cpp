#include "stdafx.h"
#include <tcode/io/engine.hpp>
#include <tcode/io/timer_id.hpp>
#include <tcode/error.hpp>
#include <mutex>

namespace tcode { namespace io { 
    
    engine::engine( void )
        : _impl(_active){
    } 

    engine::~engine( void ){
    }

    void engine::run( void ) {
        _run_thread_id = std::this_thread::get_id();
        while ( _active.count() || !_timers.empty() ){
            _impl.run( next_wake_up_time());
            timer_drain();
        }
    }

    bool engine::in_run_loop( void ) {
        return _run_thread_id == std::this_thread::get_id();    
    }

    void engine::timer_schedule( timer::id* id ){
        id->add_ref();
        if ( in_run_loop() ){
            std::list< timer::id* >::iterator it = 
                std::upper_bound( _timers.begin() , _timers.end(), id 
                    , [] ( const timer::id* v , const timer::id* cmp ) ->bool {
                        return v->due_time < cmp->due_time;
                    });
            _timers.insert( it , id );
        } else {
            execute( [this,id] {
                timer_schedule( id );
                id->release();
            });
        }
    }

    void engine::timer_cancel( timer::id* id ){
        id->add_ref();
        execute( [ this , id ] {
            auto it = std::find( _timers.begin() , _timers.end() , id );
            if ( it != _timers.end()){
                _timers.erase(it);
                id->callback( tcode::error_aborted ); 
                id->release();
            }
            id->release();
        });
    }

    void engine::timer_drain( void ) {
        tcode::timestamp ts = tcode::timestamp::now();
        while ( !_timers.empty() )
        {
            if ( _timers.front()->due_time <= ts ) {
                timer::id* id = _timers.front();
                _timers.pop_front();
                id->callback( std::error_code());
                if ( id->repeat.total_milliseconds() != 0 ) {
                    id->due_time += id->repeat;
                    timer_schedule(id);
                }                
                id->release();
            } else {
                break;
            }
        }
    }

    tcode::timespan engine::next_wake_up_time( void ){
        if ( _timers.empty())
            return tcode::timespan::seconds(1);
        tcode::timestamp now = tcode::timestamp::now();
        tcode::timestamp due = _timers.front()->due_time;
        if ( due <= now ) 
            return tcode::timespan::seconds(0);
        return due - now; 
    }

    engine::impl_type& engine::impl( void ){
        return _impl;
    }

    tcode::active_ref& engine::active( void ) {
        return _active;
    }

}}
