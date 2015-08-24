#ifndef __tcode_io_handler_h__
#define __tcode_io_handler_h__

namespace tcode { namespace io {

    typedef void (*callback)( void* userctx , void* ioctx );

    /*!
     *  @class  handler
     *  @brief  io 관련 이벤트를 사용자측에서 받기위한 클래스\n
     *  lambda 등 object 형태로 사용시 할당/해제에 대한 부담으로\n
     *  함수포인터의 wrapper 로 구현하엿음
     *  @todo   labmda , functor 형태의 구현은 helper 형태로 제공 예정
     */
    class handler {
    public:
        handler( void );
        handler( callback cb , void* userctx );
        ~handler( void );

        void operator()( void* ioctx );

        void bind( callback cb , void* userctx );
    private:
        callback _callback;
        void* _userctx;
    };

}}

#endif
