#include "stdafx.h"
#include <tcode/io/pipe.hpp>
#include <unistd.h>

namespace tcode { namespace io { 

pipe::pipe( void ){
    int p[2];
    ::pipe( p );        
    _pipe[0] = p[0];
    _pipe[1] = p[1];
}
    
pipe::~pipe( void ){
    _pipe[0].close();
    _pipe[1].close();
}

handle pipe::rd_pipe( void ){
    return _pipe[0];
}

handle pipe::wr_pipe( void ){
    return _pipe[1];
}

}}
