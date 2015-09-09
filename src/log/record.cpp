#include "stdafx.h"
#include <tcode/log/record.hpp>

namespace tcode { namespace log {

    char acronym( log::type l ){
        static log::type types[] = {
            trace , debug , info , warn , error ,fatal , all , off
        };
        static char acronyms[] = "TDIWEFAO"; 
        for ( int i = 0 ; i < 8 ; ++i ) {
            if ( l == types[i] ) return acronyms[i];
        }
        return '?';
    }

}}
