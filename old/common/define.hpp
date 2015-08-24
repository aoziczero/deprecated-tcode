#ifndef __tcode_common_define_h__
#define __tcode_common_define_h__

#include <common/compile_time_error.hpp>

//! assert msg
#ifndef TCODE_ASSERT
#define TCODE_ASSERT( expr , msg ) assert( expr && msg )
#endif

//! compile time check
#ifndef STATIC_CHECK
#define STATIC_CHECK( expr , msg ) { tcode::tmp::compile_time_error< (expr) != 0 > ERROR_##msg;(void)ERROR_##msg;}
#endif

#ifndef DISALLOW_COPY
#define DISALLOW_COPY( clazz ) \
	clazz( const clazz& rhs );\
	clazz( clazz&& rhs );\
	clazz& operator=( const clazz& rhs);\
	clazz& operator=( clazz&& rhs);
#endif

#ifndef DISALLOW_CONSTRUCTOR
#define DISALLOW_CONSTRUCTOR( clazz ) \
	clazz( void );\
	~clazz( void );
#endif

#ifndef DISALLOW_CONSTRUCTOR_AND_COPY
#define DISALLOW_CONSTRUCTOR_AND_COPY( clazz )\
	DISALLOW_CONSTRUCTOR( clazz );\
	DISALLOW_COPY( clazz );
#endif

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


/*

MSVC++ 14.0 _MSC_VER == 1900 (Visual Studio 2015)
MSVC++ 12.0 _MSC_VER == 1800 (Visual Studio 2013)
MSVC++ 11.0 _MSC_VER == 1700 (Visual Studio 2012)
MSVC++ 10.0 _MSC_VER == 1600 (Visual Studio 2010)
MSVC++ 9.0  _MSC_VER == 1500 (Visual Studio 2008)
MSVC++ 8.0  _MSC_VER == 1400 (Visual Studio 2005)
MSVC++ 7.1  _MSC_VER == 1310 (Visual Studio 2003)
MSVC++ 7.0  _MSC_VER == 1300
MSVC++ 6.0  _MSC_VER == 1200
MSVC++ 5.0  _MSC_VER == 1100

*/