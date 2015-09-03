#include "stdafx.h"
#include <tcode/io/buffer.hpp>

namespace tcode { namespace io {

    buffer::buffer( const iovec& iov )
    {
#if defined( TCODE_WIN32 )
        set( iov.buf , iov.len );
#else
        set( static_cast<char*>(iov.iov_base) , iov.iov_len );
#endif
    }

    buffer::buffer( const buffer& rhs ) 
    {
        set( rhs.buf() , rhs.len());
    }

    buffer::buffer( char* buf , int len ) 
    {
        set( buf , len );
    }

    buffer::~buffer(void){
    }

    buffer& buffer::operator=( const buffer& rhs ) {
        set(rhs.buf() , rhs.len());
    }

    char* buffer::buf( void ) const {
#if defined( TCODE_WIN32 )
        return this->buf;
#else
        return static_cast<char*>(this->iov_base);
#endif
    }

    int buffer::len( void ) const {
#if defined( TCODE_WIN32 )
        return this->len;
#else
        return this->iov_len;
#endif
    }

    void buffer::set( char* buf , int len ) {
#if defined( TCODE_WIN32 )
        this->buf = buf;
        this->len = len;
#else
        this->iov_base = buf;
        this->iov_len = len;
#endif
    }

    void buffer::skip( int n ) {
        set( buf() + n , len() - n );
    }
   

}}
