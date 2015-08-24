#include "stdafx.h"
#include <tcode/tmp/compile_time_error.hpp>

//#define _TEST_COMPILE_TIME_ERROR

TEST( compile_time_error , ok ) {
#ifdef _TEST_COMPILE_TIME_ERROR    
    STATIC_CHECK( sizeof(int) <= sizeof(char) , Int_Gt_Char);
#else
    STATIC_CHECK( sizeof(int) >= sizeof(char) , Int_Gt_Char);
#endif
}

