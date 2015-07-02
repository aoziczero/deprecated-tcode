#ifndef __tcode_threading_atomic_h__
#define __tcode_threading_atomic_h__

namespace tcode { namespace threading {

template< typename Atomic >
bool atomic_bit_on( Atomic& atomic , const int bit ){
	int exp = 0;
	int val = 0;
	atomic.compare_exchange_strong( exp , val );
	if (exp & bit)	
		return true;
	return false;
}

template< typename Atomic >
bool atomic_bit_reset( Atomic& atomic , const int bit ){
	int exp = 0;
	int val = 0;
	do {
		bool success = atomic.compare_exchange_strong( exp , val );
		if ( exp & bit ) {
			if ( success ) {
				return true;
			} else {
				val = exp ^ bit;
			}
		} else {
			return false;;
		}
	} while ( true );
}


template< typename Atomic >
bool atomic_bit_set( Atomic& atomic , const int bit ){
	int exp = 0;
	int val = 0;
	do {
		bool success = atomic.compare_exchange_strong( exp , val );
		if ( (exp & bit) == 0 ) {
			if ( success ) {
				return true;
			} else {
				val = exp | bit;
			}
		} else {
			return false;;
		}
	} while ( true );
}



}}


#endif