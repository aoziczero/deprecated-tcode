#ifndef __tcode_common_define_h__
#define __tcode_common_define_h__

#include <tcode/tmp/compile_time_error.hpp>

//! assert msg
#ifndef TCODE_ASSERT
#define TCODE_ASSERT( expr , msg ) assert( expr && msg )
#endif


//! 복사 금지
#ifndef DISALLOW_COPY
#define DISALLOW_COPY( clazz ) \
	clazz( const clazz& rhs );\
	clazz( clazz&& rhs );\
	clazz& operator=( const clazz& rhs);\
	clazz& operator=( clazz&& rhs);
#endif

//! 생성 금지
#ifndef DISALLOW_CONSTRUCTOR
#define DISALLOW_CONSTRUCTOR( clazz ) \
	clazz( void );\
	~clazz( void );
#endif

//! 복사 생성 금지
#ifndef DISALLOW_CONSTRUCTOR_AND_COPY
#define DISALLOW_CONSTRUCTOR_AND_COPY( clazz )\
	DISALLOW_CONSTRUCTOR( clazz );\
	DISALLOW_COPY( clazz );
#endif

#if defined( _WIN32 )//! win32 관련 설정
//! win32 관련 설정
    #ifndef TCODE_WIN32
	#define TCODE_WIN32
    #endif

    #ifdef _WIN64

    #else

	#endif
	
    #ifndef TCODE_ALIGN
    #define TCODE_ALIGN( n ) __declspec(align(n))
    #endif

//	#if _MSC_VER >= 1800
//	#define TCODE_CPP_0x11_SUPPORT	
//	#endif
#elif defined(__linux)
//! 리눅스 관련 설정
    #ifndef TCODE_LINUX
    #define TCODE_LINUX
    #endif 

    #ifndef _NOEXCEPT
	#define _NOEXCEPT noexcept
    #endif

	#define TCODE_ALIGN( n ) __attribute__ ((aligned(x)))

//	#if __cplusplus > 199711L
//	#define TCODE_CPP_0x11_SUPPORT		
//	#endif
#elif defined(__APPLE__)
//! 애플 관련 설정
    #ifndef TCODE_APPLE
	#define TCODE_APPLE
    #endif

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

#elif defined(__unix) // all unices not caught above
    #define TCODE_UNIX
#elif defined(__posix)
    // POSIX
#elif defined(__ANDROID__)
	#define TCODE_ANDROID
#endif

#endif

namespace tcode {

bool init( void );

}


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
