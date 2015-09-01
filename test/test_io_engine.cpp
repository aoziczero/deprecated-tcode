#include "stdafx.h"
#include <tcode/io/engine.hpp>

TEST( tcode_io_engine , ctor ){
    tcode::io::engine engine;
}


TEST( tcode_io_engine , execute ) {
    tcode::io::engine e;
    int test_val = 0;
    e.active().inc();
    e.execute( [&e , &test_val] {
                test_val = 1;
                e.active().dec();
            });
    e.run();
    ASSERT_EQ( test_val , 1 ); 
    
}
