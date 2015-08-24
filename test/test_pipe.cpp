#include "stdafx.h"
#include <tcode/io/pipe.hpp>
#include <tcode/io/option.hpp>

TEST( tcode_io_pipe , open ) {
    tcode::io::pipe pi;
    ASSERT_TRUE( pi.rd_pipe() != -1 );
    ASSERT_TRUE( pi.wr_pipe() != -1 );
}


TEST( tcode_io_pipe , nonblock ){
    tcode::io::pipe pi;
    tcode::io::option::nonblock nb;
    ASSERT_TRUE(nb.set_option( pi.rd_pipe()));
    char buf[128]; 
    ASSERT_EQ( read( pi.rd_pipe(), buf , 128 ) , -1 );

    ASSERT_EQ( errno , EAGAIN );
    ASSERT_EQ( write( pi.wr_pipe() , buf , 128 ) ,  128 );
    ASSERT_EQ( read( pi.rd_pipe() , buf , 128 ) ,  128 );
    tcode::io::option::block b;
    ASSERT_TRUE(b.set_option( pi.rd_pipe() ));
}
