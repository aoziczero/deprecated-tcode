#include "stdafx.h"
#include <tcode/log/record.hpp>

namespace tcode { namespace log {

    char acronym( log::type l ){
        static log::type types[] = {
            trace , debug , info , warn , error ,fatal , all , off
        };
        static char acronyms[] = "TDIWEFAO"; 
        for ( int i = 0 ; i < 8 ; ++i ) {
            if ( l == types[i] ) return acronyms[i];
        }
        return '?';
    }

    record::record( log::type t
                , const char* tag
                , const char* file
                , const char* function
                , const int l )
        : ts( tcode::timestamp::now()) ,  type(t) , line(l)
#if defined( TCODE_WIN32 )
          , tid( GetCurrentThreadId() )
#else
          , tid( pthread_self() )
#endif
    {
#if defined( TCODE_WIN32 )
        strcpy_s(this->tag , tag );
        strcpy_s(this->file , file  );
        strcpy_s(this->function , function );
#else
        strncpy(this->tag , tag , 32 );
        strncpy(this->file , file  , 256);
        strncpy(this->function , function , 256);
#endif
    }

    tcode::byte_buffer& operator<<( tcode::byte_buffer& buf , const record& r ) {
        buf << r.ts.tick();
        buf << r.type;
        buf << r.line;
        buf << r.tid;
        short len = (short)strlen( r.tag );    buf << len;     buf.write( const_cast<char*>(r.tag)  , len );
        len = (short)strlen( r.file );         buf << len;     buf.write( const_cast<char*>(r.file)  , len );
        len = (short)strlen( r.function );     buf << len;     buf.write( const_cast<char*>(r.function) , len );
        len = (short)strlen( r.message );      buf << len;     buf.write( const_cast<char*>(r.message)  , len );
        return buf;
    }   

    tcode::byte_buffer& operator>>( tcode::byte_buffer& buf , record& r ) {
        int64_t tick;
        buf >> tick;
        r.ts = tcode::timestamp( tick );
        buf >> r.type;
        buf >> r.line;
        buf >> r.tid;
        short len;
        buf >> len;     buf.read( r.tag , len );        r.tag[len] = 0;
        buf >> len;     buf.read( r.file  , len );      r.file[len] = 0;
        buf >> len;     buf.read( r.function  , len );  r.function[len] = 0;
        buf >> len;     buf.read( r.message  , len );   r.message[len] = 0;
        return buf;
    }   
}}
