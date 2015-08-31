#include "stdafx.h"
#include <tcode/operation.hpp>

TEST( tcode_operation , wrap )
{
    int testval = 0;
    tcode::operation* op = tcode::operation::wrap( [&] {
                testval = 1;
            });
    (*op)();
    ASSERT_EQ( testval , 1 );
}
