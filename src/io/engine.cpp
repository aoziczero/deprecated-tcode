#include "stdafx.h"
#include <tcode/io/engine.hpp>
#include <mutex>

namespace tcode { namespace io { 
    
    engine::engine( void ){
    } 

    engine::~engine( void ){
    }

    void engine::post( const operation& op ){
        _op_queue.post( op );
    }

    void engine::run( void ) {
        _op_queue.drain();
    }

    engine::op_queue::op_queue( void ) {
    }

    engine::op_queue::~op_queue( void ) {
    }

    void engine::op_queue::post( const engine::operation& op ){
         std::lock_guard< tcode::threading::spinlock > guard(_lock);
        _ops.push_back( op );
    }

    void engine::op_queue::drain( void ){
        do {
            std::lock_guard< tcode::threading::spinlock > guard(_lock);
            _swap.swap( _ops );
        }while(0);
        auto it = _swap.begin();
        while ( it != _swap.end()){
            (*it)();
            ++it;
        }
        _swap.clear();
    }
}}
