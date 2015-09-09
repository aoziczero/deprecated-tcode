#ifndef __tcode_log_record_h__
#define __tcode_log_record_h__

#include <tcode/time/timestamp.hpp>
#include <tcode/tmp/bit.hpp>

namespace tcode { namespace log {

    enum type {
        trace  = tcode::tmp::bit(0) , 
        debug  = tcode::tmp::bit(1) ,
        info   = tcode::tmp::bit(2) ,
        warn   = tcode::tmp::bit(3) ,
        error  = tcode::tmp::bit(4) ,
        fatal  = tcode::tmp::bit(5) ,
        all    = trace|debug|info|warn|error|fatal,
        off    = 0,
    };

    struct record{
        tcode::timestamp ts;
        log::type type;
        uint32_t line;
#if defined( TCODE_WIN32 )
        int tid;
#else
        pthread_t tid;
#endif
        char tag[32];
        char file[256];
        char function[256];
        char message[2048];
    };

    char acronym( log::type l );
}}

#endif
