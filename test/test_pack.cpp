#include "stdafx.h"
#include <tcode/tcode.hpp>

TEST( pack , size ) {

    TCODE_PACK( 
            struct teststruct{
                char a;
            });

    ASSERT_EQ( sizeof( teststruct ) , 1 );

}
