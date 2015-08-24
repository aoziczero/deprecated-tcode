#include "stdafx.h"
#include <tcode/io/pipe.hpp>
#include <unistd.h>

namespace tcode { namespace io { 

pipe::pipe( void ){
    ::pipe( _pipe );        
}
    
pipe::~pipe( void ){
    close(_pipe[0]);
    close(_pipe[1]);
}

int pipe::rd_pipe( void ){
    return _pipe[0];
}

int pipe::wr_pipe( void ){
    return _pipe[1];
}

}}
