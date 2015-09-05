#include "stdafx.h"
#include <tcode/error.hpp>
#include <sstream>
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

	std::error_code error_success(err_success , tcode_category()) ;
    std::error_code error_aborted(err_operation_aborted , tcode_category()) ;
    std::error_code error_disconnected( err_disconnected , tcode_category());
    std::error_code error_invalid(err_invalid, tcode_category());

    std::error_code last_error( void ) {
#if defined( TCODE_WIN32 )
		return std::error_code( WSAGetLastError() , windows_category());
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
	
#if defined( TCODE_WIN32 )
	//! 
    //! \class	windows_category_impl
    //! \brief	
    //! 
    class windows_category_impl : public std::error_category {
    public:
        //! construct
        windows_category_impl(){
        }

        //! destruct
        virtual ~windows_category_impl() noexcept
        {
        }

        //! \return "tcode_category"
        virtual const char *name() const noexcept
        {
            return "windows_category";
        }

        //! \return readable error message­
        //! \param condition [in] tcode::diagnostics::code
        virtual std::string message( int condition ) const {
            const size_t buffer_size = 4096;
			DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM;
			LPCVOID lpSource = NULL;
			char buffer[buffer_size] = {0,};
			unsigned long result;
			result = ::FormatMessageA(
				dwFlags,
				lpSource,
				condition,
				MAKELANGID( LANG_ENGLISH , SUBLANG_ENGLISH_US ),
				buffer,
				buffer_size,
				NULL);

			if (result == 0) {
				std::ostringstream os;
				os << "unknown error code: " << condition << ".";
				return os.str();
			} 
			char* pos = strrchr( buffer , '\r' );
			if ( pos ) 
				*pos = '\0';
			return std::string( buffer );
        }
    };

	std::error_category& windows_category() {
		static windows_category_impl impl;
		return impl;
	}
#endif
}
