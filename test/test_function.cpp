#include "stdafx.h"
#include <tcode/function.hpp>

void test_function( void ) {
    gout << "Call!" << gendl;
}

TEST( tcode_function , function_ptr ){
    tcode::function< void () > f( &test_function );
    f();
    tcode::function< void () > f2(f);
    f2();
    f = test_function;
    f();
    f = [] { gout << "call2" << gendl; };
    gout << "!!" << gendl;
    f();
}

TEST( tcode_function , lambda ) {
    tcode::function<void ()> f( [] {
            gout<<"Call2!"<<gendl;
            });
    f();
    tcode::function<void ()> f2(f);
    f2();
}

int test_return( int ret ){
    return ret;
}

TEST( tcode_function , return_val ) {
    tcode::function< int (int) > f(&test_return);
    ASSERT_EQ( f(2) , 2 );
    f = [] ( int ret ) {
        return ret + 2;
    };
    ASSERT_EQ( f(2) , 4 );
}

