#ifndef __tcode_base64_h__
#define __tcode_base64_h__

#include <string>

namespace tcode{ 

std::string base64_encode( const std::string& msg );
std::string base64_encode( void* ptr , std::size_t size);
int base64_decode( const std::string& src , void* ptr , std::size_t size );

}

#endif
