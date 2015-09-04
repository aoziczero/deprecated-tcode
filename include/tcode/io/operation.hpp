#ifndef __tcode_io_operation_h__
#define __tcode_io_operation_h__

#include <tcode/io/io_define.hpp>
#include <tcode/operation.hpp>

#include <system_error>

namespace tcode { namespace io {

    class operation
        : public tcode::operation 
    {
    public:
        typedef bool (*post_proc_handler)( io::operation* 
                , io::multiplexer* mux 
                , io::descriptor desc );

        operation( tcode::operation::execute_handler exh
                , post_proc_handler pph );
        bool post_proc( io::multiplexer* mux , io::descriptor desc  );

        std::error_code& error(void);
    protected:
        ~operation( void );
    private:
        std::error_code _error;
        post_proc_handler _post_proc;
#if defined ( TCODE_WIN32 )
	public:
		OVERLAPPED ov;
		DWORD& io_byte(void) {
			return ov.Offset;
		}
#endif
	};

}}


#endif
