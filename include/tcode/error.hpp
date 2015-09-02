#ifndef __tcode_error_h__
#define __tcode_error_h__

#include <system_error>

namespace tcode {

    extern std::error_code error_aborted;
    extern std::error_code error_disconnected;

    std::error_code last_error( void );
    
}

#endif
