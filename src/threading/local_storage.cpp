#include "stdafx.h"
#include <tcode/threading/local_storage.hpp>

namespace tcode { namespace threading {

    local_storage::local_storage( void )
    {
#if defined( TCODE_WIN32 )
        _key = TlsAlloc();
#else
        if ( pthread_key_create( &_key , nullptr ) < 0 ) 
            _key = -1;
#endif
    }

    local_storage::~local_storage( void )
    {
#if defined( TCODE_WIN32 )
        if ( _key != TLS_OUT_OF_INDEXES )
            TlsFree( _key );
#else
        if ( _key != -1 )
            pthread_key_delete( _key );
#endif
    }

    void* local_storage::value( void )
    {
#if defined( TCODE_WIN32 )
        if ( _key != TLS_OUT_OF_INDEXES )
           return TlsGetValue( _key );
#else
        if ( _key != -1 )
            return pthread_getspecific( _key );
#endif
        return nullptr;
    }

    void local_storage::value( void* p )
    {
#if defined( TCODE_WIN32 )
        if ( _key != TLS_OUT_OF_INDEXES )
            TlsSetValue( _key , p );
#else
        if ( _key != -1 ) 
            pthread_setspecific( _key , p );
#endif
    }

}}
