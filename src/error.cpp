#include "stdafx.h"
#include <tcode/error.hpp>

namespace tcode {

    enum errc{
        err_success = 0,
        err_disconnected,
        err_operation_aborted,
        err_invalid ,
        
        
        err_max ,
    };

    const char* errc_msg[] = {
        "success",
        "disconnected",
        "operation_aborted",
        "invalid" ,
    };

    std::error_code error_aborted(err_operation_aborted , tcode_category()) ;
    std::error_code error_disconnected( err_disconnected , tcode_category());
    std::error_code error_invalid(err_invalid, tcode_category());

    std::error_code last_error( void ) {
#if defined( TCODE_WIN32 )
		return std::error_code( WSAGetLastError() , std::generic_category());
#else
        return std::error_code( errno , std::generic_category());
#endif
    }
    
    //! 
    //! \class	tcode_category_impl
    //! \brief	
    //! 
    class tcode_category_impl : public std::error_category {
    public:
        //! construct
        tcode_category_impl(){
        }

        //! destruct
        virtual ~tcode_category_impl() noexcept
        {
        }

        //! \return "tcode_category"
        virtual const char *name() const noexcept
        {
            return "tcode_category";
        }

        //! \return readable error message­
        //! \param condition [in] tcode::diagnostics::code
        virtual std::string message( int condition ) const {
            if ( condition >= err_success && condition < err_max )
                return errc_msg[condition];
            return "unknown";
        }
    };
    
    std::error_category& tcode_category( void ) {
        static tcode_category_impl impl;
        return impl;
    }
}
