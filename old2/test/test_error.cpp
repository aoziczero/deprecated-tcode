#include "stdafx.h"
//#include <tcode/diagnostics/error.hpp>

TEST( tcode_error , error_code ) {
    std::error_code ec( ETIMEDOUT , std::generic_category());
    gout << "ETIMEDOUT : " << ec.message() << gendl;
}

