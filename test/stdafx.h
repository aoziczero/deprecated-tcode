#if defined( _WIN32 )
#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <WinSock2.h>
#include <MSWSOCK.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <winbase.h>
#include <tchar.h>
#include <dbghelp.h>
#include <process.h>
#endif

#include <stdint.h>
#include <iostream>
#include <cassert>
#include <cstring>
#include <functional>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <iterator>
#include <ctime>
#include <system_error>
//#include <common/define.hpp>
//#include <diagnostics/tcode_error_code.hpp>
#include <tcode/tcode.hpp>

#include <gtest/gtest.h>


namespace testing
{
 namespace internal
 {
  enum GTestColor {
      COLOR_DEFAULT,
      COLOR_RED,
      COLOR_GREEN,
      COLOR_YELLOW
  };
  extern void ColoredPrintf(GTestColor color, const char* fmt, ...);
 }
}
#define gprintf(...)  do {\
testing::internal::ColoredPrintf(\
	testing::internal::COLOR_GREEN, "[   USER   ] "); \
testing::internal::ColoredPrintf(\
	testing::internal::COLOR_YELLOW, __VA_ARGS__); } while(0)

class gendl_impl {
};

class gout_impl{
public:
	gout_impl& operator<< ( const std::string& msg ) {
		_message += msg;
		return *this;
	}

	gout_impl& operator<< ( const int& val ) {
		_message += std::to_string(val);
		return *this;
	}

	gout_impl& operator<< ( const std::size_t& val ) {
		_message += std::to_string(val);
		return *this;
    }
	gout_impl& operator<< ( const gendl_impl& ) {
		testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[   USER   ] ");
		testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, _message.c_str());
		testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, "\n"); 
		_message.clear();
		return *this;
	}
private:
	std::string _message;
};

static gout_impl gout;
static gendl_impl gendl;

