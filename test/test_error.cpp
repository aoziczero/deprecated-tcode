#include "stdafx.h"
//#include <tcode/diagnostics/error.hpp>

TEST( tcode_error , error_code ) {
    std::error_code ec( ETIMEDOUT , std::generic_category());
    gprintf( "%s\n" , ec.message().c_str());
}

