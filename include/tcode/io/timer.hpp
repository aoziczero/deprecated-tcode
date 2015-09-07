#ifndef __tcode_io_timer_h__
#define __tcode_io_timer_h__

#include <tcode/time/timespan.hpp>
#include <tcode/time/timestamp.hpp>
#include <tcode/function.hpp>
#include <atomic>

namespace tcode { namespace io { 

    class engine;
	/*!
		@class timer
		@brief
	*/
    class timer {
    public:
        struct id;

		//! ctor
        timer( io::engine& e );

		//! dtor
        ~timer( void );

		//! relative callback time now() + ts
        timer& due_time( const tcode::timespan& ts );
		//! absolute callback time 
        timer& due_time( const tcode::timestamp& ts );
		//! repeat tick if 0 call once
        timer& repeat( const tcode::timespan& ts );
		//! callback function
        timer& callback( const tcode::function<void (const std::error_code& ) >& cb );

		//! fire timer
        void fire( void );
		//! cancel timer 
        void cancel( void );
    private:
        timer::id* _id;
    };

}}

#endif
