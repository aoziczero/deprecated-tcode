#include "stdafx.h"
#include <tcode/threading/spinlock.hpp>

TEST( spinlock , lockunlock ){
    tcode::threading::spinlock lock;

    lock.lock();

    lock.unlock();
}
