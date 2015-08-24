#ifndef __tcode_io_h__
#define __tcode_io_h__

namespace tcode { namespace io {

    class handle {
    public:
        typedef int native_type;

        handle( void );
        explicit handle( native_type fd );
        
        handle( handle&& rhs );
        handle( const handle& rhs );

        handle operator=( native_type fd );
        handle operator=( const handle& rhs );
        handle operator=( handle&& rhs );

        ~handle( void );
        
        void close( void );

        template < typename opt >
        bool set_option( opt& o ){
            if ( good() )
                return o.set_option(native_handle());
            return false;    
        }
        template < typename opt >
        bool get_option( opt& o ){
            if ( good() )
                return o.get_option(native_handle());
            return false;
        }
        
        bool good( void ) const;
        
        native_type native_handle( void );
    private:
        native_type _handle;
    };

namespace option {

    class block{
    public:
        bool set_option( handle::native_type fd );
    };

    class nonblock{
    public:
        bool set_option( handle::native_type fd );      
    };
}

}}

#endif
