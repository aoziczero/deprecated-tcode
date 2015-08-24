#ifndef __tcode_aligned_storage_h__
#define __tcode_aligned_storage_h__

namespace tcode {

template < std::size_t sz , std::size_t al >
class aligned_storage; 

#define DECLARE_ALIGNED_STORAGE(n)\
template < std::size_t sz > class aligned_storage< sz , n >{ \
public:	\
	aligned_storage( void ){}\
	~aligned_storage( void ){}\
	void* data( void ) const {\
		return reinterpret_cast< void* >(\
			const_cast< unsigned char* >(_data));\
	}\
	std::size_t size( void ) const { return sz;}\
	DISALLOW_COPY( aligned_storage )\
private:\
	TCODE_ALIGN(n) unsigned char _data[sz];\
};\

DECLARE_ALIGNED_STORAGE(4);
DECLARE_ALIGNED_STORAGE(8);
DECLARE_ALIGNED_STORAGE(16);

}

#endif