#ifndef __tcode_threading_spinlock_h__
#define __tcode_threading_spinlock_h__

#include <tcode/tcode.hpp>

#if defined( TCODE_WIN32 )

#elif defined( TCODE_APPLE )
#include <libkern/OSAtomic.h>
#else
#include <pthread.h>
#endif

namespace tcode { namespace threading {
    /*!
     *  @class  spinlock
     *  @bried  os 별 spinlock wrapper
     */
    class spinlock{
    public:
        spinlock( void );
        ~spinlock( void );

        void lock( void );
        void unlock( void );
        bool trylock( void );

		/*!
		*  @class  guard
		*  @bried  raii lock guard
		*/
        class guard{
        public:
            guard( spinlock& lock )
                : _lock( lock )
            {
                _lock.lock();
            }

            ~guard( void ) {
                _lock.unlock();
            }
        private:
            spinlock& _lock;
        };

    private:
#if defined( TCODE_WIN32 )
        CRITICAL_SECTION _lock;
#elif defined( TCODE_APPLE )
        OSSpinLock _lock;
#else
        pthread_spinlock_t _lock;
#endif
    };

}}

#endif
