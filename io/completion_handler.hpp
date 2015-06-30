#ifndef __tcode_io_completion_handler_h__
#define __tcode_io_completion_handler_h__

#include <common/slist.hpp>
#include <diagnostics/tcode_error_code.hpp>

namespace tcode { namespace io {

class completion_handler 
#if defined( TCODE_TARGET_WINDOWS )	
	: public OVERLAPPED
#endif
{
public:
	completion_handler( void );
	virtual ~completion_handler( void );
	
	virtual void operator()( const tcode::diagnostics::error_code& ec 
			, const int completion_bytes ) = 0;
	// contain error_code for post completion
	tcode::diagnostics::error_code error_code( void );
	void error_code( const tcode::diagnostics::error_code& ec );
		
	void prepare( void );
	
	TCODE_SLIST_NODE_DECL( completion_handler );	
private:
#if !defined( TCODE_TARGET_WINDOWS )	
	tcode::diagnostics::error_code _error_code;
#endif
};

}}

#endif