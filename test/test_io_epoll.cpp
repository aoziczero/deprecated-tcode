#include "stdafx.h"
#include <tcode/io/io_define.hpp>
#include <tcode/io/pipe.hpp>
#include <thread>
#include <tcode/io/engine.hpp>
/*
TEST( tcode_io_epoll , ctor ){
    tcode::io::epoll epoll;

    tcode::io::pipe pipe;

    int evt = 0;
    tcode::function< void (int)> event_handler( [&evt]( int ev ){
        evt = ev; 
    });



    epoll.bind( pipe.rd_pipe() , tcode::io::EV_READ , &event_handler );

    char  b = 0;
    write( pipe.wr_pipe()  , &b , 1 );

    epoll.run( tcode::timespan::seconds(1));
    ASSERT_EQ( evt , tcode::io::EV_READ );
    epoll.unbind( pipe.rd_pipe());

}
*/
TEST( tcode_io_mux , wake_up ){
    tcode::io::engine e;
    tcode::io::multiplexer mux(e);
    std::thread t( [&mux]{
                mux.run( tcode::timespan::seconds(60));
            });

    mux.wake_up();
    t.join();
}

TEST( tcode_io_mux , execute ){
    tcode::io::engine e;
    tcode::io::multiplexer mux(e);
    mux.execute( tcode::operation::wrap( [] {
                }));
    ASSERT_EQ( 1,mux.run( tcode::timespan::seconds(1)));

}
