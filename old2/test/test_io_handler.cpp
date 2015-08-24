#include "stdafx.h"
#include <tcode/io/handler.hpp>

void call( void* userctx , void* ioctx ){
    int* val = reinterpret_cast<int*>(userctx);
    *val = *val + 1;
}

TEST( tcode_io_handler , ctor ){
    int testvalue = 0;
    tcode::io::handler handler( &call , &testvalue);
    handler(nullptr);
    ASSERT_EQ( testvalue , 1 );
}

TEST( tcode_io_handler , bind ){
    int testvalue = 0;
    tcode::io::handler handler;
    handler( nullptr );
    handler.bind( &call , &testvalue );
    handler( nullptr );
    ASSERT_EQ( testvalue , 1 );
}
