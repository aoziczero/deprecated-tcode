#ifndef __tcode_operation_h__
#define __tcode_operation_h__

#include <tcode/slist.hpp>
#include <tcode/allocator.hpp>

namespace tcode {

    /*!
     * @class operation
     * @brief 
     */
    class operation
        : public tcode::slist::node< operation >
    {
    public:
        //! 가상함수 대용 함수 포인터
        typedef void (*execute_handler)( operation* );
    public:
        //! ctor
        operation( execute_handler fn );
        
        //! execute_handler 호출
        void operator()( void );

    public:
        //! void () 형태의 함수 wrap
        //! operation::allocator 사용
        template < typename Handler >
        static operation* wrap( const Handler& h );

        //! operation 용 할당함수 
        static void* alloc( int sz ); 
        //! operation 용 해제함수 
        static void free( void* p );
        //! operation allocator
        static tcode::allocator& allocator(void);
    protected:
        //! 소멸자 상속클래스로만 delete 가능
        ~operation( void );
    private:
        //! 실제 호출 함수 
        execute_handler _execute;
    };


    /*!
     * @class operation_impl
     * @brief void () 형 호출 operation wrapper class
     */
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
