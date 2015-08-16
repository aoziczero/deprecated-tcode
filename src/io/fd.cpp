#include "stdafx.h"
#include <tcode/io/fd.hpp>
#include <unistd.h>
#include <fcntl.h>

namespace tcode { namespace io { 

fd::fd( int handle )
    :_handle(handle)
{

}

fd::~fd( void ){

}

int fd::handle( void ){
    return _handle;
}


int fd::handle( int new_handle ){
    int old = _handle;
    _handle = new_handle;
    return old;
}

int fd::close( void ){
    int fd = handle(-1);
    if ( fd != -1 ) 
        return ::close(fd);
    return -1;
}

int fd::blocking( void ) {
    int x = fcntl(handle(),F_GETFL , 0 );
    return fcntl(handle() , F_SETFL , x & ~O_NONBLOCK );
}

int fd::non_blocking( void ) {
    int x = fcntl(handle(),F_GETFL , 0 );
    return fcntl(handle() , F_SETFL , x | O_NONBLOCK );
}

}}

