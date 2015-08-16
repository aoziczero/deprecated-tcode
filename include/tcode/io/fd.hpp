#ifndef __tcode_io_fd_h__
#define __tcoce_io_fd_h__

namespace tcode { namespace io { 

/*!
    @class  fd
    @brief  리눅스 fd 간단 wrapper
    소유권은 관리 하지 않는다
*/  
class fd {
public:
    //! 생성자
    //! @param handle 설정할 handle값
    explicit fd( int handle = -1 );
    //! 소멸자
    ~fd( void );

    //! handle getter
    //! @return handle 값
    int handle( void );

    //! handle setter
    //! @param new_handle
    //! @return 이전의 handle 값
    int handle( int new_handle );

    //! fd close
    //! @return 0 성공 -1 실패
    int close( void );

    //! blocking
    //! @return 0 성공 -1 실패
    int blocking( void );

    //! non_blocking
    //! @return 0 성공 -1 실패
    int non_blocking( void );
private:
    int _handle; //!< fd 값
};

}}

#endif
