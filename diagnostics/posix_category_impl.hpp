//! 
//! \file	posix_category_impl.hpp
//! \brief	POSIX ���� ī�װ� ����
//! 
#ifndef __posix_category_impl_h__
#define __posix_category_impl_h__

namespace tcode{
namespace diagnostics{

//! 
//! \class	posix_category_impl
//! \brief	POSIX ���� ī�װ� ����
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 06
//! 
class posix_category_impl : public error_category{
public:
	
	//! construct
	posix_category_impl();

	//! destruct
	virtual ~posix_category_impl() _NOEXCEPT;

	//! category �ĺ��̸�
	//! \return "posix_category"
	virtual const char *name() const _NOEXCEPT;

	//!
	//! \return POSIX ���� ���ڿ�	
	//! \brief strerror_* �Լ� ���
	//! \param _Errval [in] POSIX error code ( errno )
	//!
	virtual std::string message( int _Errval ) const;
};

//! posix_category ref
std::error_category& posix_category();

}}


#endif