#include "stdafx.h"
#include <tcode/log/file_writer.hpp>
#include <tcode/time/timestamp.hpp>
#include <tcode/time/timespan.hpp>
#include <tcode/time/ctime_adapter.hpp>
#include <tcode/time/systemtime.hpp>
#include <tcode/string.hpp>

#if defined( TCODE_WIN32 )
#else
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/param.h>
#include <pwd.h>
extern char* program_invocation_name;
extern char* program_invocation_short_name;

#endif

namespace tcode { namespace log {

file_writer::file_writer( void ) 
#if defined( TCODE_WIN32 )
	: _file(INVALID_HANDLE_VALUE) ,
#else
	: _file(nullptr) ,
#endif
	_current_day(0) , 
	_line_count(0) , 
	_day_file_count(0){
}

file_writer::~file_writer( void ) {

}

void file_writer::write( const record& r ) {
	tcode::time::systemtime st(tcode::timestamp::now());
#if defined( TCODE_WIN32 )
	if( _file == INVALID_HANDLE_VALUE 
#else
	if( _file == nullptr 
#endif
        ||  _current_day  != st.wDay || _line_count > 65535 ) {

		if( _current_day != st.wDay ) {
			_delete_old_logs();
			_day_file_count = 0;
		}

		if( !_create_log_file() )
			return ;

		++_day_file_count;
		_line_count = 0;
        _current_day = st.wDay;
	}
    ++_line_count;
	formatter()->format( r , _buffer );
#if defined( TCODE_WIN32 )
	DWORD sz = 0;
	::WriteFile( _file , _buffer.rd_ptr() , static_cast< DWORD >(_buffer.length()) , &sz , nullptr );
	FlushFileBuffers(_file);
#else
	fwrite( _buffer.rd_ptr() , 1 , _buffer.length() , _file  );
	fflush(_file);
#endif
	_buffer.clear();
}

#if defined( TCODE_WIN32 )
bool file_writer::_create_log_file( void ) {
	if ( _file != INVALID_HANDLE_VALUE ) 
		CloseHandle( _file );

	wchar_t filePath[MAX_PATH];
	GetModuleFileName( nullptr , filePath , MAX_PATH );
	wchar_t *fileName = wcsrchr( filePath , '\\' );
	if ( fileName ) {
		*(++fileName) = '\0';
	}else{
		memset( filePath , 0x00 , MAX_PATH * sizeof( wchar_t ));
	}
	
	std::wstring path = filePath;
    path.append( L"\\log\\" );

    LPSECURITY_ATTRIBUTES attr = nullptr;
    CreateDirectory( path.c_str() , attr );

	tcode::time::systemtime st( tcode::timestamp::now());
    wchar_t file_name[4096];
    swprintf_s( file_name , 
                L"log_%04d%02d%02d_%02d%02d%02d_%04d.log" ,
                st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond ,
                _day_file_count );
    
    path.append( file_name );
	
    _file = CreateFile(  path.c_str() , 
                                GENERIC_READ | GENERIC_WRITE , 
                                FILE_SHARE_READ | FILE_SHARE_WRITE , 
                                NULL , 
                                CREATE_ALWAYS , 
                                FILE_ATTRIBUTE_NORMAL , 
                                NULL );

	if( _file == INVALID_HANDLE_VALUE )	
        return false;
/*
	unsigned char bom[2];
	bom[0] = 0xff;
	bom[1] = 0xfe;
	DWORD sz = 0;
	::WriteFile( _file , bom , 2 , &sz , nullptr );
*/
    return true;
}


void file_writer::_delete_old_logs( void ) {
	wchar_t filePath[MAX_PATH];
	GetModuleFileName( nullptr , filePath , MAX_PATH );
	wchar_t *fileName = wcsrchr( filePath , '\\' );
	if ( fileName ) {
		*(++fileName) = '\0';
	}else{
		memset( filePath , 0x00 , MAX_PATH * sizeof( wchar_t ));
	}
	
	std::wstring path = filePath;
    path.append( L"\\log\\" );

	tcode::timestamp sep( tcode::timestamp::now() - tcode::timespan::days(30));
	tcode::time::systemtime st( tcode::timestamp::now());

    wchar_t file_name[4096];
	swprintf_s( file_name , L"log_%04d%02d%02d" , st.wYear , st.wMonth , st.wDay );

	std::wstring sep_file_name( file_name );

	std::wstring find_file_name;
    find_file_name.reserve( 4096 );

    WIN32_FIND_DATA find_data;
    HANDLE h = FindFirstFile( (path + L"*.*").c_str() , &find_data );
	if( h != INVALID_HANDLE_VALUE ){
		do {
            if ( !(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                find_file_name = find_data.cFileName;
				if( sep_file_name > find_file_name ) {
                    find_file_name = path + find_data.cFileName;
					DeleteFile( find_file_name.c_str());
				}
            }
		} while ( FindNextFile( h , &find_data ) == TRUE );
		FindClose( h );
	}
}
#else

bool file_writer::_create_log_file( void ) {
    if ( _file != nullptr ) {
        fclose( _file );
    }
    struct passwd* pw = getpwuid( getuid());
    std::string path( pw->pw_dir );
    path.append( "/logs/" );
    mkdir( path.c_str() , 0755 );
    path.append( program_invocation_short_name );
    mkdir( path.c_str() , 0755 );

	tcode::time::systemtime st( tcode::timestamp::now());
    tcode::string::append_format( path 
            ,"/%04d%02d%02d_%02d%02d%02d_%04d.log" 
            , st.wYear , st.wMonth , st.wDay , st.wHour , st.wMinute , st.wSecond 
            , _day_file_count );
    _file = fopen( path.c_str() , "wb" );
	if( _file == nullptr )	
        return false;
    return true;
}


void file_writer::_delete_old_logs( void ) {
    struct passwd* pw = getpwuid( getuid());
    std::string path( pw->pw_dir );
    path.append( "/logs/" );
    path.append( program_invocation_short_name );

    tcode::timestamp sep( tcode::timestamp::now() - tcode::timespan::days(30));
	tcode::time::systemtime st( tcode::timestamp::now());

    std::string delete_before;
    tcode::string::append_format( delete_before 
            ,"%04d%02d%02d" 
            , st.wYear , st.wMonth , st.wDay );
    struct stat statinfo;
    if ( stat( path.c_str() , &statinfo ) != 0 ) {
        return;
    } 

    if ( !S_ISDIR( statinfo.st_mode ) ) {
        return;
    }

    DIR* dir = opendir( path.c_str()); 
    if ( dir == nullptr )
        return;

    char full_path [4096];
    struct dirent* ent = nullptr;
    while ( ( ent = readdir( dir )) != nullptr ) {
        sprintf( full_path , "%s/%s" , path.c_str() , ent->d_name );
        if ( stat( full_path , &statinfo ) == 0 ) {
            if ( !S_ISDIR( statinfo.st_mode )){
                if ( std::string( ent->d_name ) < delete_before ) {
                    remove( full_path );
                }
            }
        }
    }
    closedir( dir );
}


#endif

}}
