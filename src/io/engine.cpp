#include "stdafx.h"
#include <tcode/io/engine.hpp>
#include <tcode/io/timer_id.hpp>
#include <tcode/error.hpp>
#include <mutex>

namespace tcode { namespace io { 
    
    engine::engine( void ){
        _active.store(0);
    } 

    engine::~engine( void ){
    }

    void engine::post( const operation& op ){
        _active.fetch_add(1);
        _op_queue.post( op );
        _impl.wake_up();
    }

    void engine::run( void ) {
        _run_thread_id = std::this_thread::get_id();
        while ( _active.load() || !_timers.empty() ){
            _impl.run( next_wake_up_time());
            int drain = _op_queue.drain();
            _active.fetch_sub( drain );
            timer_drain();
        }
    }

    bool engine::in_run_loop( void ) {
        return _run_thread_id == std::this_thread::get_id();    
    }
    bool engine::bind( int fd , int ev , tcode::io::event_handler* handler){
        return _impl.bind( fd , ev , handler );
    }
    void engine::unbind( int fd ) {
        _impl.unbind(fd);
    }
    
    void engine::active_inc( void ){
        _active.fetch_add(1);
    }
    void engine::active_dec( void ){
        _active.fetch_sub(1);
    }

    void engine::timer_schedule( timer::id* id ){
        if ( in_run_loop() ){
            id->add_ref();
            std::list< timer::id* >::iterator it = 
                std::upper_bound( _timers.begin() , _timers.end(), id 
                    , [] ( const timer::id* v , const timer::id* cmp ) ->bool {
                        return v->due_time < cmp->due_time;
                    });
            _timers.insert( it , id );
        } else {
            id->add_ref();
            post( [this,id] {
                timer_schedule( id );
                id->release();
            });
        }
    }

    void engine::timer_cancel( timer::id* id ){
        id->add_ref();
        post( [ this , id ] {
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
        if ( due  <= now ) 
            return tcode::timespan::seconds(0);
        return due - now; 
    }

    engine::op_queue::op_queue( void ) {
    }

    engine::op_queue::~op_queue( void ) {
    }

    void engine::op_queue::post( const engine::operation& op ){
         std::lock_guard< tcode::threading::spinlock > guard(_lock);
        _ops.push_back( op );
    }

    int engine::op_queue::drain( void ){
        do {
            std::lock_guard< tcode::threading::spinlock > guard(_lock);
            _swap.swap( _ops );
        }while(0);
        int cnt = static_cast<int>(_swap.size());
        auto it = _swap.begin();
        while ( it != _swap.end()){
            (*it)();
            ++it;
        }
        _swap.clear();
        return cnt;
    }

}}
