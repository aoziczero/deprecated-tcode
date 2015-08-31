#include "stdafx.h"
#include <tcode/threading/spinlock.hpp>

TEST( spinlock , lockunlock ){
    tcode::threading::spinlock lock;

    lock.lock();
    lock.unlock();
}
/*
TEST( spinlock , guard ) {
    tcode::threading::spinlock lock;

    {
        auto guard = lock.guard();
        ASSERT_TRUE( lock._locked );
    }
    ASSERT_FALSE( lock._locked );
}*/
