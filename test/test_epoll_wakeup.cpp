#include "stdafx.h"
#include <sys/epoll.h>
#include <thread>
#include <unistd.h>
#include <tcode/io/pipe.hpp>

TEST( tcode_epoll_wakeup , stdout_wakeup ){
    int ep = epoll_create(256);
    int nfd = 0;
    std::thread t( [&]{
                struct epoll_event ev[4];
                nfd = epoll_wait( ep , ev , 4 , 100000000 );
            });

    struct epoll_event e;
    e.events = EPOLLOUT | EPOLLONESHOT;
    e.data.ptr = nullptr;
    epoll_ctl( ep , EPOLL_CTL_ADD , 1 , &e );
    t.join();
    ASSERT_EQ( nfd , 1 );
    std::thread t2 ([&] {
        struct epoll_event ev[4];
        nfd = epoll_wait( ep , ev , 4 , 100 );
    });
    t2.join();
    ASSERT_EQ( nfd , 0 );
    std::thread t3 ([&] {
        struct epoll_event ev[4];
        nfd = epoll_wait( ep , ev , 4 , 1000000000 );
    });
    ASSERT_TRUE( epoll_ctl( ep , EPOLL_CTL_ADD , 1 , &e ) != 0 );
    ASSERT_TRUE( epoll_ctl( ep , EPOLL_CTL_MOD , 1 , &e ) == 0 );
    t3.join();
    ASSERT_EQ( nfd , 1 );
    close( ep );    
}



TEST( tcode_epoll_wakeup , pipe_wakeup ){

    tcode::io::pipe p;
    
    int ep = epoll_create(256);
    int nfd = 0;
    std::thread t( [&]{
                struct epoll_event ev[4];
                nfd = epoll_wait( ep , ev , 4 , 100000000 );
            });

    struct epoll_event e;
    e.events = EPOLLOUT | EPOLLONESHOT;
    e.data.ptr = nullptr;
    epoll_ctl( ep , EPOLL_CTL_ADD , p.wr_pipe()  , &e );

    t.join();
    ASSERT_EQ( nfd , 1 );
    std::thread t2 ([&] {
        struct epoll_event ev[4];
        nfd = epoll_wait( ep , ev , 4 , 100 );
    });

    t2.join();

    ASSERT_EQ( nfd , 0 );
    std::thread t3 ([&] {
        struct epoll_event ev[4];
        nfd = epoll_wait( ep , ev , 4 , 1000000000 );
    });

    ASSERT_TRUE( epoll_ctl( ep , EPOLL_CTL_ADD , p.wr_pipe() , &e ) != 0 );
    ASSERT_TRUE( epoll_ctl( ep , EPOLL_CTL_MOD , p.wr_pipe() , &e ) == 0 );

    t3.join();
    ASSERT_EQ( nfd , 1 );
    close( ep );    
}
