#include "stdafx.h"
#include <tcode/io/pipe.hpp>
#include <unistd.h>
#include <fcntl.h> 

namespace tcode { namespace io { 

pipe::pipe( void ){
    int p[2];
    pipe2( p , O_NONBLOCK );        
    _pipe[0].handle(p[0]);
    _pipe[1].handle(p[1]);
}
    
pipe::~pipe( void ){
    _pipe[0].close();
    _pipe[1].close();
}

tcode::io::fd& pipe::rd_pipe( void ){
    return _pipe[0];
}

tcode::io::fd& pipe::wr_pipe( void ){
    return _pipe[1];
}

}}
