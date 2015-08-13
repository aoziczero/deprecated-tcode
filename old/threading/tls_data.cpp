#include "stdafx.h"
#include "tls_data.hpp"
#include "tls.hpp"

namespace tcode { namespace threading { 
namespace detail {

static tls tcode_tls;

}

tls_data* tls_data::instance( void ) {
	return static_cast<tls_data*>( detail::tcode_tls.value() );
}

tls_data* tls_data::instance( tls_data* td ){
	tls_data* old = instance();
	detail::tcode_tls.value( td );
	return old;
}


}}