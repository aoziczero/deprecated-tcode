#ifndef __tcode_tmp_compile_time_error_h__
#define __tcode_tmp_compile_time_error_h__

namespace tcode {
namespace tmp {

template < bool > class compile_time_error;
template <> class compile_time_error< true >{};


}}

#endif