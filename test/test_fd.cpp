#include "stdafx.h"
#include <tcode/io/fd.hpp>
#include <unistd.h>

TEST( tcode_fd , handle ) {
    tcode::io::fd fd(-1);
    ASSERT_EQ( fd.handle(1) , -1 );
    ASSERT_EQ( fd.handle(-1) , 1 );
}

TEST( tcode_fd , close_success ) {
    int p[2];
    ASSERT_FALSE( pipe(p) < 0 );
    tcode::io::fd rfd(p[0]);
    tcode::io::fd wfd(p[1]);
    ASSERT_EQ( rfd.close() , 0 );
    ASSERT_EQ( wfd.close() , 0 );
}

TEST( tcode_fd , fcntl ) {
    int p[2];
    ASSERT_FALSE( pipe(p) < 0 );
    tcode::io::fd rfd(p[0]);
    tcode::io::fd wfd(p[1]);
    ASSERT_EQ( rfd.blocking() , 0 );
    ASSERT_EQ( rfd.non_blocking() , 0 );
    ASSERT_EQ( rfd.close() , 0 );
    ASSERT_EQ( wfd.close() , 0 );
}
