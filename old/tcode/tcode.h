#ifndef __tcode_h__
#define __tcode_h__

#include <stdint.h>

typedef int8_t	s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t	 u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;



#if defined( _WIN32 )
	//define something for Windows (32-bit and 64-bit, this part is common)
	#define TCODE_TARGET_WINDOWS
	#ifdef _WIN64
		//define something for Windows (64-bit only)
	#else

	#endif
	#define TCODE_ALIGN( n ) __declspec(align(n))
	#if _MSC_VER >= 1800
	#define TCODE_CPP_0x11_SUPPORT	
	#endif


#elif defined(__APPLE__)
	#define TCODE_TARGET_MACOSX
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
         // iOS Simulator
    #elif TARGET_OS_IPHONE
        // iOS device
    #elif TARGET_OS_MAC
        // Other kinds of Mac OS
    #else
        // Unsupported platform
    #endif
#elif defined(__linux)
    #define TCODE_TARGET_LINUX
	#define _NOEXCEPT noexcept
	#define TCODE_ALIGN( n ) __attribute__ ((aligned(x)))
	#if __cplusplus > 199711L
	#define TCODE_CPP_0x11_SUPPORT		
	#endif
#elif defined(__unix) // all unices not caught above
    #define TCODE_TARGET_UNIX
#elif defined(__posix)
    // POSIX
#elif defined(__ANDROID__)
	#define TCODE_TARGET_ANDROID
#endif

#endif

