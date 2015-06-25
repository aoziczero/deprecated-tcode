#ifndef __tcode_container_of_h__
#define __tcode_container_of_h__

namespace tcode{

//! boost intrusive ÂüÁ¶
template<class Parent , class Member >
std::ptrdiff_t offset_of(const Member Parent::*ptr_to_member)
{
	const Parent * const parent = 0;
	const char *const member = static_cast<const char*>(
			static_cast<const void*>(&(parent->*ptr_to_member)));
	std::ptrdiff_t val(member - static_cast<const char*>(
			static_cast<const void*>(parent)));
	return val;
}

template<class Parent, class Member>
inline Parent* container_of(Member *member, const Member Parent::* ptr_to_member)
{
   return static_cast<Parent*>(
		   	  static_cast<void*>(
		   			  static_cast<char*>(static_cast<void*>(member)) - offset_of(ptr_to_member)
         ));
}

}

#endif
