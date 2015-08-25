#include "stdafx.h"
#include <tcode/timer_queue.hpp>

TEST( timer_queue , timer ) {
    tcode::timer_queue tqueue;
    tcode::timer_queue::timer* timer = tqueue.timer( 
                tcode::timestamp::now() + tcode::timespan::microseconds(1)
            ,   tcode::timespan::microseconds(1)
            ,   []( ) {
                    gout << "timer!!" << gendl;
                });

    timer->start();
    timer->stop();
    timer->release();
}
