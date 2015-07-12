#ifndef __tcode_diagnostics_seh_h__
#define __tcode_diagnostics_seh_h__

#include <DbgHelp.h>

namespace tcode { namespace diagnostics {

class mini_dump{
public:
	mini_dump( MINIDUMP_TYPE type );
	~mini_dump(void);

	bool dump( const std::wstring& file_name 
			,  EXCEPTION_POINTERS *pExp );

	// 다른 핸들러를 덮어 버리므로 주의
	void install_seh( void );
private:
	typedef BOOL (WINAPI *pf_write_dump)(
	    HANDLE hProcess, 
        DWORD dwPid, 
        HANDLE hFile, 
        MINIDUMP_TYPE DumpType,
	    CONST PMINIDUMP_EXCEPTION_INFORMATION	ExceptionParam,
	    CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	    CONST PMINIDUMP_CALLBACK_INFORMATION	CallbackParam
	);
	pf_write_dump _write_dump;
	HMODULE		_dbg_help_dll;
	MINIDUMP_TYPE _type;
};


}}

#endif