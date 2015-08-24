#ifndef __tcode_tmp_compile_time_error_h__
#define __tcode_tmp_compile_time_error_h__

namespace tcode { namespace tmp{

/*! 
    @class  complie_time_error
    @brief  complie_time_error  의 선언
  */
template < bool >
class compile_time_error;

/*!
    @class  compile_time_error<true>
    @brief  compile_time_error<true> 구현으로
    false 에 해당하면 compile 시 에러가 발생한다.
 */
template < >
class compile_time_error< true >{
};

}}

#ifndef STATIC_CHECK
#define STATIC_CHECK( expr , msg ) do { tcode::tmp::compile_time_error< (expr) != 0 > Error_##msg;(void)Error_##msg;  } while(0);
#endif

#endif
