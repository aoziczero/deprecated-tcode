#ifndef __tcode_threading_last_error_h__
#define __tcode_threading_last_error_h__

namespace tcode { namespace diagnostics {

tcode::diagnostics::error_code last_error( void );
void last_error( const tcode::diagnostics::error_code& ec );

}}

#endif