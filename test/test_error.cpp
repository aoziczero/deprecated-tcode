#include "stdafx.h"
#include <tcode/error.hpp>

TEST( tcode_error , error_code ) {
#if defined( TCODE_WIN32 )
	std::error_code ec( ERROR_TIMEOUT , tcode::windows_category());
    gout << "ETIMEDOUT : " << ec.message() << gendl;
#else
    std::error_code ec( ETIMEDOUT , std::generic_category());
    gout << "ETIMEDOUT : " << ec.message() << gendl;
#endif
}

