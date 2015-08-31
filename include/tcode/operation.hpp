#ifndef __tcode_operation_h__
#define __tcode_operation_h__

#include <tcode/slist.hpp>

namespace tcode {

    class operation
        : public tcode::slist::node< operation>
    {
    public:
        typedef void (*execute_impl)( operation* );
        operation( execute_impl fn );
        void operator()( void );

        template < typename Handler >
        static operation* wrap( const Handler& h );

        static void* alloc( int sz ) {
            return operator new (sz);
        }
        static void free( void* p ){
            operator delete(p); 
        }
    protected:
        ~operation( void );
    private:
        execute_impl _execute;
    };


    template < typename Handler >
    class operation_impl : public operation{
    public:
        operation_impl( const Handler& h ) 
            : operation( &operation_impl::execute )
            , _handler( h ) 
        {
        }
        ~operation_impl( void ){
        }
        
        static void execute( operation* op_base ) {
            operation_impl* op( static_cast< operation_impl* >(op_base));
            Handler h(std::move( op->_handler ));
            op->~operation_impl();
            operation::free( op );
            h();
        }
    private:
        Handler _handler;
    };

    template < typename Handler >
    operation* operation::wrap( const Handler& h ) {
        void* ptr = tcode::operation::alloc( sizeof( operation_impl<Handler> ));
        new (ptr) operation_impl<Handler>(h);
        return reinterpret_cast< operation* >(ptr);
    }
}

#endif
