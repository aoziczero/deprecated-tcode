#include "stdafx.h"
#include <tcode/string.hpp>
#include <vector>
#include <stdarg.h>

namespace tcode{
namespace string {

#if defined( TCODE_WIN32 )

std::string  wcs_to_mbs( const std::wstring& in ) {
	std::string out;
    std::size_t length = in.length();
    if ( length != 0 ) {
        int numOfChar = WideCharToMultiByte( CP_ACP , 0 , in.c_str() , -1 , nullptr , 0 , nullptr , nullptr );
        if ( numOfChar != 0 ) {
            std::vector< char > result(numOfChar);
            WideCharToMultiByte( CP_ACP , 0 , in.c_str() , -1 , &result[0] , numOfChar , nullptr , nullptr );
            out = &result[0];
        }
    }
    return out;
}

std::wstring mbs_to_wcs( const std::string&  in ) {
	std::wstring out;
    std::size_t length = in.length();
    if ( length != 0 ) {
        int numOfChar = MultiByteToWideChar( CP_ACP , 0 , in.c_str() , -1 , nullptr , 0 );
        if ( numOfChar != 0 ) {
            std::vector< wchar_t > result(numOfChar);
            MultiByteToWideChar( CP_ACP , 0 , in.c_str() , -1 , &result[0] , numOfChar );
            out = &result[0];
        }
    }
    return out;
}

std::string  mbs_to_utf_8( const std::string& in ){
	return wcs_to_utf_8( mbs_to_wcs( in ));
}

std::string  wcs_to_utf_8( const std::wstring& in ) {
	std::string out;
	std::size_t length = in.length();
	if ( length != 0 ) {
		int numOfChar =WideCharToMultiByte( CP_UTF8 , 0 , in.c_str()  , -1  , nullptr , 0  , nullptr , nullptr );
		if ( numOfChar != 0 ) {
			std::vector< char > result(numOfChar);
			WideCharToMultiByte( CP_UTF8  , 0 , in.c_str() , -1  ,  &result[0] , numOfChar , nullptr , nullptr );
			out = &result[0];
		}
	}
	return out;
}

std::wstring utf_8_to_wcs( const std::string& in ){
	std::wstring out;
    std::size_t length = in.length();
    if ( length != 0 ) {
		int numOfChar = MultiByteToWideChar( CP_UTF8 , 0 , in.c_str() , -1 , nullptr , 0 );
        if ( numOfChar != 0 ) {
            std::vector< wchar_t > result(numOfChar);
            MultiByteToWideChar( CP_UTF8 , 0 , in.c_str() , -1 , &result[0] , numOfChar );
            out = &result[0];
        }
    }
    return out;
}

std::string  utf_8_to_mbs( const std::string& in ) {
	return wcs_to_mbs( utf_8_to_wcs( in ));
}

std::string& append_format( std::string& msg , const char* format , ... ) {
	if ( format != nullptr ) {
		char fmt_buffer[4096];
		va_list args;
		va_start( args , format );
		int ret = vsnprintf_s( fmt_buffer , 4096 , format , args );
		va_end( args );
		if (ret >= 4096 || ret < 0)
			fmt_buffer[4095] = '\0';
		msg += fmt_buffer;
	}
	return msg;
}

std::wstring& append_format( std::wstring& msg , const wchar_t* format , ... ) {
	if ( format != nullptr ) {
		wchar_t fmt_buffer[4096];
		va_list args;
		va_start( args , format );
		int ret = _vsnwprintf_s( fmt_buffer , 4096 , format , args );
		va_end( args );
		if (ret >= 4096 || ret < 0)
			fmt_buffer[4095] = '\0';
		msg += fmt_buffer;
	}
	return msg;
}

namespace {
	static wchar_t ChoSungTable[] = L"ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ";
	static wchar_t JungSungTable[] = L"ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅘㅙㅚㅛㅜㅝㅞㅟㅠㅡㅢㅣ";
	static wchar_t JongSungTable[] = L"ㄱㄲㄳㄴㄵㄶㄷㄹㄺㄻㄼㄽㄾㄿㅀㅁㅂㅄㅅㅆㅇㅈㅊㅋㅌㅍㅎ";
	static wchar_t HangulUnicodeStart = 0xAC00;
	static wchar_t HangulUnicodeEnd   = 0xD79F;
}

bool string::is_hangul( wchar_t ch ) {
	if ( ch >= HangulUnicodeStart && ch <= HangulUnicodeEnd ) return true;
	return false;
}

string::hangul::hangul( const wchar_t cho , const wchar_t jung , const wchar_t jong ) 
	: _ChoSung( cho ) , _JungSung( jung ) , _JongSung( jong ) {

}

string::hangul::~hangul( void ) {

}

wchar_t string::hangul::cho_sung() const {
	return _ChoSung;
}
wchar_t string::hangul::jung_sung() const{
	return _JungSung;
}
wchar_t string::hangul::jong_sung() const {
	return _JongSung;
}

string::hangul string::extract_hangul( wchar_t ch ) {
	int unicodeIndex  = ch - HangulUnicodeStart;
    int jongsungIndex = unicodeIndex % 28;  unicodeIndex /= 28;
    int jungsungIndex = unicodeIndex % 21;  unicodeIndex /= 21;
    int chosungIndex  = unicodeIndex;
	return hangul( ChoSungTable[ chosungIndex ] , JungSungTable[ jungsungIndex ] , JongSungTable[ jongsungIndex ] );
}

#else

std::string& append_format( std::string& msg , const char* format , ... ) {
	if ( format != nullptr ) {
		char fmt_buffer[4096];
		va_list args;
		va_start( args , format );
		int ret = vsnprintf( fmt_buffer , 4096 , format , args );
		va_end( args );
		if (ret >= 4096 || ret < 0)
			fmt_buffer[4095] = '\0';
		msg += fmt_buffer;
	}
	return msg;
}

#endif

std::string raw_to_hex_string(void* p, int sz) {
	static const char hex[] = "0123456789ABCDEF";
	std::string value;
	unsigned char* data_ptr = static_cast< unsigned char* >(p);
	for (int i = 0; i < sz; ++i) {
		value += hex[data_ptr[i] >> 4];
		value += hex[data_ptr[i] & 0x0f];
	}
	return value;
};
}}
