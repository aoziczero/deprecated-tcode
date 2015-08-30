#include "stdafx.h"
#include <sys/epoll.h>
#include <tcode/io/pipe.hpp>

TEST( tcode_epoll_et , read ){
    int epoll = epoll_create( 1 );
    
    tcode::io::pipe pipe;

    struct epoll_event e;
    e.events = EPOLLIN | EPOLLET;
    e.data.fd = pipe.rd_pipe();

    epoll_ctl( epoll , EPOLL_CTL_ADD , pipe.rd_pipe() , &e );

    ASSERT_EQ( 0 ,  epoll_wait( epoll , &e , 1 , 100 ));

    char ch = 0;

    ASSERT_EQ( 1 , write( pipe.wr_pipe() , &ch , 1 ));
    ASSERT_EQ( 1 , write( pipe.wr_pipe() , &ch , 1 ));

    ASSERT_EQ( 1 , epoll_wait( epoll , &e , 1 , 100 ));
    ASSERT_EQ( e.data.fd , pipe.rd_pipe());

    ASSERT_EQ( 1 , read( pipe.rd_pipe() , &ch , 1 ));
    ASSERT_EQ( 0 , epoll_wait( epoll , &e , 1 , 100 ));

    ASSERT_EQ( 1 , write( pipe.wr_pipe() , &ch , 1 ));

    ASSERT_EQ( 1 , epoll_wait( epoll , &e , 1 , 100 ));

    ASSERT_EQ( 1 , read( pipe.rd_pipe() , &ch , 1 ));
    ASSERT_EQ( 1 , read( pipe.rd_pipe() , &ch , 1 ));

    close( epoll );

}


TEST( tcode_epoll_et , write ){
    int epoll = epoll_create( 1 );
    
    tcode::io::pipe pipe;

    struct epoll_event e;
    e.events = EPOLLOUT | EPOLLET;
    e.data.fd = pipe.rd_pipe();

    epoll_ctl( epoll , EPOLL_CTL_ADD , pipe.wr_pipe() , &e );

    ASSERT_EQ( 1 ,  epoll_wait( epoll , &e , 1 , 100 ));

    char ch = 0;

    ASSERT_EQ( 1 , write( pipe.wr_pipe() , &ch , 1 ));
    ASSERT_EQ( 1 , write( pipe.wr_pipe() , &ch , 1 ));

    ASSERT_EQ( 0 , epoll_wait( epoll , &e , 1 , 100 ));
    ASSERT_EQ( 1 , read( pipe.rd_pipe() , &ch , 1 ));
    ASSERT_EQ( 0 , epoll_wait( epoll , &e , 1 , 100 ));

    ASSERT_EQ( 1 , write( pipe.wr_pipe() , &ch , 1 ));

    ASSERT_EQ( 0 , epoll_wait( epoll , &e , 1 , 100 ));

    ASSERT_EQ( 1 , read( pipe.rd_pipe() , &ch , 1 ));
    ASSERT_EQ( 1 , read( pipe.rd_pipe() , &ch , 1 ));
    close( epoll );

}
