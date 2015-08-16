#include "stdafx.h"
#include <tcode/io/pipe.hpp>


TEST( tcode_pipe , open ) {
    tcode::io::pipe pi;
    ASSERT_TRUE( pi.rd_pipe().handle() != -1 );
    ASSERT_TRUE( pi.wr_pipe().handle() != -1 );
}
