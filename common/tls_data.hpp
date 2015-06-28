#ifndef __tcode_tls_data_h__
#define __tcode_tls_data_h__

namespace tcode { namespace threading { 

struct tls_data {
	std::error_code error;	
	static tls_data* instance( void );
	static tls_data* instance( tls_data* td );
};

}}

#endif