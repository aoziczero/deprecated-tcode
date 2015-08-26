#include "stdafx.h"
#include <tcode/io/engine.hpp>

TEST( timer , call ) {
    tcode::io::engine e;
    //ASSERT_TRUE( tcode::io::id_alloc.load() ==  0 );
    //ASSERT_TRUE( tcode::io::id_free.load() ==  0 );
    ASSERT_TRUE( tcode::io::test_alive_ids() == 0 );
    {
        tcode::io::timer t(e);
        ASSERT_TRUE( tcode::io::test_alive_ids() == 1 );
        t.due_time( tcode::timespan::seconds(1))
         .repeat( tcode::timespan::milliseconds(200))
         .callback( [] ( const std::error_code& ec )
                 {
                    if ( ec ) 
                    {
                        gout << ec.message() << gendl; 
                        return;
                    }
                    gout << "Timer!" << gendl;
                 });
        t.fire();
        //ASSERT_TRUE( tcode::io::id_alloc.load() ==  1 );
        //ASSERT_TRUE( tcode::io::id_free.load() ==  0 );
    
        std::thread thread( [&t] {
                    sleep(2);
                    t.cancel();
                });

        e.run();
        thread.join();
    }
    ASSERT_TRUE( tcode::io::test_alive_ids() == 0 );
    //gout << tcode::io::id_add_ref.load() << gendl;
    //gout << tcode::io::id_release.load() << gendl;
    //ASSERT_EQ( tcode::io::id_alloc.load() , 1 );
    //ASSERT_EQ( tcode::io::id_free.load() , 1 );
            
}
