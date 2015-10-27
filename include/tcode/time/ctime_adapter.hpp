#ifndef __tcode_ctime_adapter_h__
#define __tcode_ctime_adapter_h__

#include <tcode/time/timestamp.hpp>

namespace tcode {

    /*!
     *  @class ctime_adapter
     *	@brief timestamp �� ctime ���� ��ȯ ó��
	 *	@author	tk
     */
    class ctime_adapter {
    public:
        ctime_adapter( const timestamp& ts );

        time_t tick( void ) const;
        
        std::string ctime(void);
        std::string strftime( const std::string& fmt , bool local = false );
    private:
        timestamp _timestamp;
    };

}

#endif
