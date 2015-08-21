#ifndef __tcode_timestamp_h__
#define __tcode_timestamp_h__

#include <stdint.h>

namespace tcode {

    class timespan;
    /*!
     *  @class timestamp
     *  @brief 1970년 1 월 1 일 0 시 이후 microsecond 단위로 1식 증가 
     *  utc
     */
    class timestamp {
    public:
        explicit timestamp(const int64_t tick);
        timestamp( const timestamp& rhs );
        timestamp& operator=( const timestamp& rhs );

        int64_t tick( void ) const;
        
        bool operator==( const timestamp& rhs ) const;
        bool operator!=( const timestamp& rhs ) const;
        bool operator> ( const timestamp& rhs ) const;
        bool operator>=( const timestamp& rhs ) const;
        bool operator< ( const timestamp& rhs ) const;
        bool operator<=( const timestamp& rhs ) const;

        timestamp operator+( const timespan& rhs ) const;
        timestamp operator-( const timespan& rhs ) const;
        timestamp& operator+=( const timespan& rhs ) ;
        timestamp& operator-=( const timespan& rhs ) ;

        timespan  operator-( const timestamp& rhs ) const;
        
        std::string to_string( void ) const;
        
        static timestamp now( void );
    private:
        int64_t _tick;
    };

}

#endif
