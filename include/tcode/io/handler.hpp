#ifndef __tcode_io_handler_h__
#define __tcode_io_handler_h__

namespace tcode { namespace io {

    typedef void (*callback)( void* userctx , void* ioctx );

    /*!
     *  @class  handler
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
