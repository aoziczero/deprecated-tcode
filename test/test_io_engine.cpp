#include "stdafx.h"
#include <tcode/io/engine.hpp>

TEST( tcode_io_engine , ctor ){
    tcode::io::engine engine;
}

TEST( tcode_io_engine , op_queue ) {
    tcode::io::engine engine;
    int val = 0;
    engine.post( [&val] {
        ++val;
    });
    engine.post( [&val] {
        ++val;
    });
    engine.run();
    ASSERT_EQ( val , 2 );
}
