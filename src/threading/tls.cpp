#include "stdafx.h"
#include "tls.hpp"

namespace tcode { namespace threading {

#if defined(TCODE_TARGET_WINDOWS)

tls::tls(void) {
	_key = TlsAlloc();
	if (_key == TLS_OUT_OF_INDEXES) {
	}
}

tls::~tls(void){
	if (_key != TLS_OUT_OF_INDEXES)
		TlsFree(_key);
	_key = TLS_OUT_OF_INDEXES;
}

void* tls::value(void* v) {
	void* old = value();
	if (_key != TLS_OUT_OF_INDEXES)
		TlsSetValue(_key, v);
	return old;
}

void* tls::value(void) {
	if (_key != TLS_OUT_OF_INDEXES)
		return TlsGetValue(_key);
	return nullptr;
}

#else

tls::tls(void) {
	switch (pthread_key_create(&_key, nullptr)) {
	case EAGAIN:
		break;
	}
}

tls::~tls(void){
	pthread_key_delete(_key);
}

void* tls::value(void* v) {
	void* old = value();
	pthread_setspecific(_key, v);
	return old;
}

void* tls::value(void) {
	return pthread_getspecific(_key);
}

#endif

}}
