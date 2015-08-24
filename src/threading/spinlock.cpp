#include "stdafx.h"
#include <tcode/threading/spinlock.hpp>

namespace tcode { namespace threading {

#if defined( TCODE_WIN32 )

spinlock::spinlock( void ) {
    BOOL ret = InitializeCriticalSectionAndSpinCount( &_lock , 4000 );
}

spinlock::~spinlock( void ) {
    DeleteCriticalSection( &_lock );
}

void spinlock::lock( void ) {
	EnterCriticalSection( &_lock );
}

void spinlock::unlock( void ){
	LeaveCriticalSection( &_lock );
}

bool spinlock::trylock( void ) {
    return TryEnterCriticalSection( &_lock ) == TRUE;
}

#elif defined( TCODE_APPLE )

spinlock::spinlock( void ) {
    _lock = OS_SPINLOCK_INIT;
}

spinlock::~spinlock( void ) {
}

void spinlock::lock( void ) {
    OSSpinLockLock( &_lock );
}

void spinlock::unlock( void ){
	OSSpinLockUnlock( &_lock );
}

bool spinlock::trylock( void ) {
    return OSSpinLockTry( &_lock );
}

#else

spinlock::spinlock( void ) {
    pthread_spin_init( &_lock , PTHREAD_PROCESS_PRIVATE );
}

spinlock::~spinlock( void ) {
    pthread_spin_destroy( &_lock );
}

void spinlock::lock( void ) {
	pthread_spin_lock( &_lock );
}

void spinlock::unlock( void ){
	pthread_spin_unlock( &_lock );
}

bool spinlock::trylock( void ) {
    return  pthread_spin_trylock( &_lock ) == 0;
}

#endif


}}
