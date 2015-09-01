#include "stdafx.h"
#include <tcode/io/epoll.hpp>
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
TEST( tcode_io_epoll , wake_up ){
    tcode::active_ref e;
    tcode::io::epoll epoll(e);
    std::thread t( [&epoll]{
                epoll.run( tcode::timespan::seconds(60));
            });

    epoll.wake_up();
    t.join();
}

TEST( tcode_io_epoll , execute ){
    tcode::active_ref e;
    tcode::io::epoll ep(e);
    ep.execute( tcode::operation::wrap( [] {
                }));
    ASSERT_EQ( 1,ep.run( tcode::timespan::seconds(1)));

}
