#ifndef __tcode_begin_pack_h__
#define __tcode_begin_pack_h__

#include <tcode/tcode.hpp>

#if defined( TCODE_WIN32 )

    #ifdef PACKED
    #undef PACKED
    #endif
    
    #define PACKED

    #pragma pack(push,1)

#else

    #ifdef PACKED
    #undef PACKED
    #endif

    #define PACKED __attribute__((__packed__))

#endif

#endif
