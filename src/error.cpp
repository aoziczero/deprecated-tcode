#include "stdafx.h"
#include <tcode/error.hpp>

namespace tcode {

    std::error_code error_aborted(ECANCELED , std::generic_category()) ;

    std::error_code last_error( void ) {
        return std::error_code( errno , std::generic_category());
    }
}
