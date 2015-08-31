#include "stdafx.h"
#include <tcode/io/engine.hpp>

TEST( timer , call ) {
    tcode::io::engine e;
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
    
        std::thread thread( [&t] {
                    sleep(2);
                    t.cancel();
                });

        e.run();
        thread.join();
    }
    ASSERT_TRUE( tcode::io::test_alive_ids() == 0 );
}
