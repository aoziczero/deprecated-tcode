#include "stdafx.h"
#include <tcode/io/pipe.hpp>

TEST( tcode_io_pipe , open ) {
    tcode::io::pipe pi;
    ASSERT_TRUE( pi.rd_pipe().good() );
    ASSERT_TRUE( pi.wr_pipe().good() );
}


TEST( tcode_io_pipe , nonblock ){
    tcode::io::pipe pi;
    tcode::io::option::nonblock nb;
    ASSERT_TRUE(pi.rd_pipe().set_option( nb ));
    char buf[128]; 
    ASSERT_EQ( read( pi.rd_pipe().native_handle() , buf , 128 ) , -1 );

    ASSERT_EQ( errno , EAGAIN );
    ASSERT_EQ( write( pi.wr_pipe().native_handle() , buf , 128 ) ,  128 );
    ASSERT_EQ( read( pi.rd_pipe().native_handle() , buf , 128 ) ,  128 );
    tcode::io::option::block b;
    ASSERT_TRUE(pi.rd_pipe().set_option( b ));
}
