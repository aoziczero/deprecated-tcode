#include "stdafx.h"
#include "mini_dump.hpp"
#include <thread>
#include <common/time_stamp.hpp>
#include <common/time_util.hpp>

namespace tcode { namespace diagnostics {
namespace detail {

static mini_dump* instance;

static void __stdcall write_dump( EXCEPTION_POINTERS *pExp ){
	wchar_t path[MAX_PATH];

	GetModuleFileNameW( NULL, path , MAX_PATH * sizeof(wchar_t)); 
    wchar_t *ext = wcsrchr( path , '.' );

	SYSTEMTIME st;
	tcode::time::convert_to( tcode::time_stamp::now() , st );

    wsprintf( ext ? ext : path + wcslen( path ) , 
				L"_%04d%02d%02d_%02d%02d%02d_ProcessId_%d.dmp" , 
				st.wYear , st.wMonth , st.wDay ,
				st.wHour , st.wMinute, st.wMinute ,
				GetCurrentProcessId());

	instance->dump( std::wstring( path ) , pExp );
}

static LONG __stdcall handle_exception( EXCEPTION_POINTERS *pExp ) {
	if ( pExp->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW ) {
		std::thread thr([pExp](){
			write_dump(pExp);
		});
		thr.join();
	} else {
		write_dump(pExp);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

}


mini_dump::mini_dump( MINIDUMP_TYPE type )
	: _type(type) , _dbg_help_dll( LoadLibrary( _T("DBGHELP.DLL") ) ) 
	, _write_dump( nullptr )
{
	if ( _dbg_help_dll != nullptr ) {
		_write_dump = reinterpret_cast< pf_write_dump  >( 
									GetProcAddress( _dbg_help_dll , "MiniDumpWriteDump" ));
	}
}


mini_dump::~mini_dump(void)
{

}

bool mini_dump::dump( const std::wstring& file_name 
			,  EXCEPTION_POINTERS *pExp)
{
	
	if ( _write_dump == nullptr ) {
		return false;
	}

	HANDLE file = CreateFileW(  file_name.c_str() , 
			                    GENERIC_READ | GENERIC_WRITE, 
                                0, 
                                NULL, 
                                CREATE_ALWAYS, 
                                FILE_ATTRIBUTE_NORMAL, 
                                NULL );

    if ( file == INVALID_HANDLE_VALUE ) {
        return false;
	}
    
    MINIDUMP_EXCEPTION_INFORMATION ExInfo;

	ExInfo.ThreadId           = GetCurrentThreadId(); 
	ExInfo.ExceptionPointers  = pExp; 
	ExInfo.ClientPointers     = FALSE; 

	BOOL ret = _write_dump(GetCurrentProcess(), 
				GetCurrentProcessId(), 
				file, 
				_type , 
				&ExInfo , 
				NULL , 
				NULL );

    CloseHandle( file );
	return ret == TRUE;
}

void mini_dump::install_seh( void ){
	detail::instance = this;	
	SetUnhandledExceptionFilter( &detail::handle_exception );
}

}}