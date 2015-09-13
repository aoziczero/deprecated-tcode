#include "stdafx.h"
#include <tcode/threading/local_storage.hpp>
#include <tcode/error.hpp>
#include <thread>

tcode::threading::local_storage tls;
tcode::threading::local_storage_T< std::error_code > last_error;

TEST( tcode_threading_local_storage , t ){
    ASSERT_EQ( tls.value() , nullptr );
    tls.value((void*)0x01);

    void* v;
    void* v2;
    std::thread t( [&v,&v2] {
                v = tls.value();
                tls.value((void*)0x02);
                v2 = tls.value();
            });
    t.join();

    ASSERT_EQ( v , nullptr );
    ASSERT_EQ( v2 , (void*)0x02);
    ASSERT_EQ( tls.value() , (void*)0x01);
}

TEST( tcode_threading_local_storage_T , last_error ) {
    last_error.value() = tcode::error_aborted;
}
