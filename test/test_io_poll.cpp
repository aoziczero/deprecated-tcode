#include "stdafx.h"
#include <tcode/io/poll.hpp>
#include <tcode/io/pipe.hpp>
#include <thread>
#include <tcode/io/io.hpp>

TEST( tcode_io_poll , ctor ){
    tcode::io::poll poll;

    tcode::io::pipe pipe;

    int evt = 0;
    tcode::function< void (int)> event_handler( [&evt]( int ev ){
        evt = ev; 
    });

    poll.bind( pipe.rd_pipe() , tcode::io::EV_READ , &event_handler );

    char  b = 0;
    write( pipe.wr_pipe()  , &b , 1 );

    poll.run( tcode::timespan::seconds(1));
    ASSERT_EQ( evt , tcode::io::EV_READ );
    poll.unbind( pipe.rd_pipe());

}

TEST( tcode_io_poll , wake_up ){
    tcode::io::poll poll;
    std::thread t( [&poll]{
                poll.run( tcode::timespan::seconds(60));
            });

    poll.wake_up();
    t.join();
}
