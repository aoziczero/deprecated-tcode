#ifndef __tcode_active_ref_h__
#define __tcode_active_ref_h__

#include <atomic>

namespace tcode {

    class active_ref{
    public:
        active_ref(void);
        ~active_ref(void);
        int inc( void );
        int dec( void );
        int count( void );
    private:
        std::atomic<int> _count;
    };

}

#endif
