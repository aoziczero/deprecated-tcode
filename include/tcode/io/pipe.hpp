#ifndef __tcode_io_pipe_h__
#define __tcode_io_pipe_h__

#include <tcode/io/fd.hpp>

namespace tcode { namespace io { 

/*!
    @class  pipe
    @biref  pipe 의 wrapper\n
    multiplexer 에서 \n
    self pipe trick 사용용도로 구현\n
    nonblock    
*/  
class pipe {
public:
    //! 생성자
    pipe( void );
    //! 소멸자
    ~pipe( void );

    //! 읽기용 fd
    int rd_pipe( void );
    //! 쓰기용 fd
    int wr_pipe( void );
private:
    int _pipe[2];
};

}}

#endif
