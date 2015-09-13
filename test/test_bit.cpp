#include "stdafx.h"

template < typename T , typename R = uint32_t >
constexpr R bit( T idx ){
    return static_cast<R>( 1u << (R)idx );
}

enum bits {
    a = bit(0),
    b = bit(1),
    c = bit(2),
};
TEST( bit_mask , t ){

   ASSERT_EQ( a , 1 );
   ASSERT_EQ( b , 1 << 1 );

}


enum class flagidx {
    flaga,
    flagb,
    flagc,
};


enum flag{
    flaga=bit(flagidx::flaga),
    flagb=bit(flagidx::flagb),
};

TEST( bit_index_flag , t ){
    ASSERT_EQ( (int)flagidx::flaga , 0 );
    ASSERT_EQ( flag::flaga , 1 );
    ASSERT_EQ( (int)flagidx::flagb , 1 );
    ASSERT_EQ( flag::flagb , 2  );

}
