//! 
//! \file	tcode_category_impl.hpp
//! \brief	���̺귯�� ���� ī�װ� ����	
//! 
#ifndef __tcode_category_impl_h__
#define __tcode_category_impl_h__

#include <common/tcode_error_code.hpp>

namespace tcode{
namespace diagnostics{

//! 
//! \class	tcode_category_impl
//! \brief	error_condition �� category
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 06
//! 
class tcode_category_impl 
: public std::error_category {
public:
	//! construct
	tcode_category_impl();

	//! destruct
	virtual ~tcode_category_impl() _NOEXCEPT;

	//! category �ĺ��̸�
	//! \return "tcode_category"
	virtual const char *name() const _NOEXCEPT;

	//! \return ���̺귯�� ���� ���� ���ڿ�
	//! \param condition [in] tcode::diagnostics::code
	virtual std::string message( int condition ) const;

	//! \brief error_code �� ��� ��
	//! \param errcode   [in] std::error_code
	//! \param condition [in] tcode::diagnostics::code
	virtual bool equivalent(const std::error_code& errcode,
		int condition) const _NOEXCEPT;

};

//! tcode_category ref
std::error_category& tcode_category();


}}

namespace std{

//! error_condition �����Լ�
error_condition make_error_condition( tcode::diagnostics::error_code condition );
//error_code make_error_code( tcode::diagnostics::code code );

//!
//! \brief error_code �� �� �뵵\n
//! \n
//! ���� �� �ڵ尡\n
//! \n
//! std::error_code success;\n
//! if(success == tcode::diagnostics::code::success){\n
//! }\n
//! \n
//! �� ������ \n
//! \n
//! template<class _Enum,\n
//!	class = typename enable_if<is_error_condition_enum<_Enum>::value,void>::type>\n
//!	error_condition(_Enum _Errcode) _NOEXCEPT\n
//!	: _Myval(0), _Mycat(0)\n
//!	{	// construct from enumerated error code\n
//!		*this = make_error_condition(_Errcode);	// using ADL\n
//!	}\n
//! \n
//! �����ڷ� matching ���� tcode::diagnostics::code �� \n 
//! error_condtion ���� ��ȯ��Ű�� �뵵
//!
template <>
  struct is_error_condition_enum<tcode::diagnostics::error_code>
    : public true_type {};

/*
error_condition ���� ����Ұ�
template <>
  struct is_error_code_enum<tcode::diagnostics::code>
    : public true_type {};
*/
}

#endif