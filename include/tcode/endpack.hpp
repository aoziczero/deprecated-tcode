#ifndef __tcode_end_pack_h__
#define __tcode_end_pack_h__

#include <tcode/tcode.hpp>

#if defined( TCODE_WIN32 )

    #pragma pack(pop)

    #ifdef PACKED
    #undef PACKED
    #endif

#else

    #ifdef PACKED
    #undef PACKED
    #endif

#endif

#endif
