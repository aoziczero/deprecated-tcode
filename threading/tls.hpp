#ifndef __tcode_tls_h__
#define __tcode_tls_h__

namespace tcode { namespace threading {

class tls {
public:
	tls(void);
	~tls(void);

	void*  value(void* value);
	void*  value(void);
private:
#if defined(TCODE_TARGET_WINDOWS)
	typedef DWORD key_type;
#else
	typedef pthread_key_t key_type;
#endif
	key_type _key;
};

}}

#endif