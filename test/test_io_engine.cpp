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

TEST( tcode_io_engine , pipe ) {
    tcode::io::engine engine;
    tcode::io::pipe pipe;

    int evt = 0;
    tcode::io::event_handler event_handler( [&evt , &engine]( int ev ){
        evt = ev; 
        engine.active_dec();
    });

    engine.bind( pipe.rd_pipe() 
            , tcode::io::EV_READ 
            , &event_handler );

    engine.active_inc();

    char  b = 0;
    write( pipe.wr_pipe()  , &b , 1 );

    engine.run();
    ASSERT_EQ( evt , tcode::io::EV_READ );
    engine.unbind( pipe.rd_pipe());

}
