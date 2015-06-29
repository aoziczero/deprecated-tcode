//! 
//! \file	windows_category_impl.hpp
//! \brief	������ ���� ī�װ� ����	
//! 
#ifndef __windows_category_impl_h__
#define __windows_category_impl_h__

namespace tcode{
namespace diagnostics{

//! 
//! \class	windows_category_impl
//! \brief	������ ���� ī�װ� ����
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 06
//! 
class windows_category_impl : public error_category {
public:
	//! construct
	windows_category_impl();

	//! destruct
	virtual ~windows_category_impl() _NOEXCEPT;

	//! category �ĺ��̸�
	//! \return "windows_category"
	virtual const char *name() const _NOEXCEPT;

	//!
	//! \brief FormatMessage �Լ��� �̿��Ͽ�\n
	//! ������ �����ڵ带 ���ڿ��� ��ȯ�Ѵ�.
	//! \return ������ ���� ���ڿ�	
	//! \param _Errval [in] windows error code ( (WSA)GetLastError())
	//!
	virtual std::string message( int _Errval ) const;

};


//! windows_category ref
std::error_category& windows_category();

}}


#endif