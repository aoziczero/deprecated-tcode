#ifndef __tcode_io_operation_h__
#define __tcode_io_operation_h__

#include <tcode/operation.hpp>

#include <system_error>

namespace tcode { namespace io {

    class operation
        : public tcode::operation 
    {
    public:
        typedef bool (*post_proc_handler)( io::operation* , void* desc );

        operation( tcode::operation::execute_handler exh
                , post_proc_handler pph );
        bool post_proc( void* desc );

        std::error_code& error(void);
    protected:
        ~operation( void );
    private:
        std::error_code _error;
        post_proc_handler _post_proc;
    };

}}


#endif
