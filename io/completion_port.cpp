#include "stdafx.h"
#include "completion_port.hpp"
#include "completion_handler.hpp"

#include <diagnostics/tcode_error_code.hpp>


namespace tcode {
namespace io {
namespace {

const int WAKE_UP = -1;

}

completion_port::completion_port( const int num_of_proc ) {
	int nop = num_of_proc;
	if ( nop ==0 ) {
		SYSTEM_INFO sysinfo;
		GetSystemInfo( &sysinfo );
		nop = sysinfo.dwNumberOfProcessors;
	}
	handle(CreateIoCompletionPort(
			INVALID_HANDLE_VALUE
		,	NULL
		,	0
		,   nop ));
}

completion_port::~completion_port( void ){
	HANDLE old = handle( INVALID_HANDLE_VALUE );
	if ( old != INVALID_HANDLE_VALUE )
		CloseHandle( old );
}

int completion_port::run( const tcode::time_span& ts ) {
	LPOVERLAPPED    overlapped;
    DWORD           num_of_trans_bytes;
    void*	        completion_key;
    bool result = GetQueuedCompletionStatus( handle() , 
									&num_of_trans_bytes ,
									reinterpret_cast< PULONG_PTR >( &completion_key ) ,
									&overlapped,
                                    (DWORD)ts.total_milliseconds() ) == TRUE;
	if ( overlapped == nullptr ) {
		if ( !result ) {
			//the function did not dequeue a completion packet from the completion port
			/* 
			if (  GetLastError() == ERROR_ABANDONED_WAIT_0 )
				// handle closed while the call is outstanding
				return -1;
			*/
			return -1;
		}	
		if ( num_of_trans_bytes == WAKE_UP ) {
			tcode::slist::queue< tcode::io::completion_handler > drains;
			do {
				tcode::threading::scoped_lock<> guard( _lock );
				drains.add_tail( std::move( _posted ));
			}while(0);
			while( !drains.empty()) {
				tcode::io::completion_handler* handler = drains.front();
				drains.pop_front();
				(*handler)(handler->error_code() , 0 );
			}
		}
	} else {
		tcode::io::completion_handler* handler = static_cast< tcode::io::completion_handler* >( overlapped );
		std::error_code ec;
		if ( !result ) {
			ec = tcode::diagnostics::platform_error();
		}
		(*handler)( ec , num_of_trans_bytes );
	}
	return 0;
}


bool completion_port::bind( SOCKET h ) {
	if ( CreateIoCompletionPort( (HANDLE)h , handle() ,(ULONG_PTR)h , 0 ) == handle() ) 
	{
		return true;
	}   
	return false;
}

void completion_port::unbind( SOCKET ){
	
}

void completion_port::wake_up() {
	PostQueuedCompletionStatus( handle() ,
            WAKE_UP , 
            0 , 
            nullptr);
}


void completion_port::post( tcode::io::completion_handler* op ){
	tcode::threading::scoped_lock<> guard( _lock );
	_posted.add_tail( op );
	wake_up();
}

}}
