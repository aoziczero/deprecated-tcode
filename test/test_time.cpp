#include "stdafx.h"
#include <tcode/time/timestamp.hpp>
#include <tcode/time/ctime_adapter.hpp>
#include <tcode/time/timespan.hpp>

TEST( tcode_timestamp , now ){
    tcode::timestamp ts = tcode::timestamp::now();
    tcode::ctime_adapter ct(ts);
    gout << ct.ctime() << gendl;
    gout << ts.to_string() << gendl;

}

TEST( tcode_timestamp , timespan ){
    tcode::timestamp ts1 = tcode::timestamp::now();
    gout << "1:" <<  (ts1 + tcode::timespan::days(0) ).to_string(false) << gendl;
    gout << "2:" <<  (ts1 + tcode::timespan::days(2) ).to_string(false) << gendl;
    gout << "3:" <<  (ts1 + tcode::timespan::days(2) ).to_string(false) << gendl;
}
