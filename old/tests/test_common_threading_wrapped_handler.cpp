#include "stdafx.h"
/*

#include <common/wrapped_handler.hpp>
#include <thread>

TEST( wrapped_handler , thread_init ) {
	bool ins =false;
	std::thread test_thread(	
		tcode::threading::bind(	
		[&ins] {
			if ( tcode::threading::tls_data::instance() == nullptr ) {
				ins = false;
			} else {
				ins = true;
			}
		}));
	test_thread.join();
	ASSERT_TRUE( ins );
	
}*/