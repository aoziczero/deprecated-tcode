#include "stdafx.h"
#include <diagnostics/last_error.hpp>
#include <future>

TEST( last_err , get_set ) {
	tcode::diagnostics::error_code ec = tcode::diagnostics::last_error();
	ASSERT_FALSE( ec );
	tcode::diagnostics::last_error( tcode::diagnostics::cancel );
	ASSERT_EQ( tcode::diagnostics::cancel , tcode::diagnostics::last_error());
	std::thread test_thread( [&ec] {
		ec = tcode::diagnostics::last_error();	
		
	});
	test_thread.join();
	ASSERT_FALSE( ec );

}