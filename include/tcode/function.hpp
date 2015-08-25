#ifndef __tcode_function_h__
#define __tcode_function_h__

#include <atomic>

namespace tcode {

#define TCODE_FUNCTION_TD_0 typename R
#define TCODE_FUNCTION_TD_1 TCODE_FUNCTION_TD_0 , typename A1
#define TCODE_FUNCTION_TD_2 TCODE_FUNCTION_TD_1 , typename A2
#define TCODE_FUNCTION_TD_3 TCODE_FUNCTION_TD_2 , typename A3
#define TCODE_FUNCTION_TD_4 TCODE_FUNCTION_TD_3 , typename A4
#define TCODE_FUNCTION_TD_5 TCODE_FUNCTION_TD_4 , typename A5
                               
    // template specialization parameter
#define TCODE_FUNCTION_TSP_0
#define TCODE_FUNCTION_TSP_1 A1
#define TCODE_FUNCTION_TSP_2 TCODE_FUNCTION_TSP_1 , A2
#define TCODE_FUNCTION_TSP_3 TCODE_FUNCTION_TSP_2 , A3
#define TCODE_FUNCTION_TSP_4 TCODE_FUNCTION_TSP_3 , A4
#define TCODE_FUNCTION_TSP_5 TCODE_FUNCTION_TSP_4 , A5
                                
    // template specialization	
#define TCODE_FUNCTION_TS_0 R ( TCODE_FUNCTION_TSP_0 )
#define TCODE_FUNCTION_TS_1 R ( TCODE_FUNCTION_TSP_1 )
#define TCODE_FUNCTION_TS_2 R ( TCODE_FUNCTION_TSP_2 )
#define TCODE_FUNCTION_TS_3 R ( TCODE_FUNCTION_TSP_3 )
#define TCODE_FUNCTION_TS_4 R ( TCODE_FUNCTION_TSP_4 )
#define TCODE_FUNCTION_TS_5 R ( TCODE_FUNCTION_TSP_5 )

    // template parameter
#define TCODE_FUNCTION_TP_0
#define TCODE_FUNCTION_TP_1	typename forward_traits< A1 >::type p1 
#define TCODE_FUNCTION_TP_2 TCODE_FUNCTION_TP_1 , typename forward_traits< A2 >::type p2
#define TCODE_FUNCTION_TP_3 TCODE_FUNCTION_TP_2 , typename forward_traits< A3 >::type p3
#define TCODE_FUNCTION_TP_4 TCODE_FUNCTION_TP_3 , typename forward_traits< A4 >::type p4
#define TCODE_FUNCTION_TP_5 TCODE_FUNCTION_TP_4 , typename forward_traits< A5 >::type p5

    // template forward
#define TCODE_FUNCTION_TF_0
#define TCODE_FUNCTION_TF_1	p1
#define TCODE_FUNCTION_TF_2 TCODE_FUNCTION_TF_1 , p2
#define TCODE_FUNCTION_TF_3 TCODE_FUNCTION_TF_2 , p3
#define TCODE_FUNCTION_TF_4 TCODE_FUNCTION_TF_3 , p4
#define TCODE_FUNCTION_TF_5 TCODE_FUNCTION_TF_4 , p5

#define TCODE_FUNCTION_TP2_0 void* ctx
#define TCODE_FUNCTION_TP2_1 TCODE_FUNCTION_TP2_0 , typename forward_traits< A1 >::type p1 
#define TCODE_FUNCTION_TP2_2 TCODE_FUNCTION_TP2_1 , typename forward_traits< A2 >::type p2
#define TCODE_FUNCTION_TP2_3 TCODE_FUNCTION_TP2_2 , typename forward_traits< A3 >::type p3
#define TCODE_FUNCTION_TP2_4 TCODE_FUNCTION_TP2_3 , typename forward_traits< A4 >::type p4
#define TCODE_FUNCTION_TP2_5 TCODE_FUNCTION_TP2_4 , typename forward_traits< A5 >::type p5

#define TCODE_FUNCTION_TF2_0 _context
#define TCODE_FUNCTION_TF2_1 TCODE_FUNCTION_TF2_0 , p1
#define TCODE_FUNCTION_TF2_2 TCODE_FUNCTION_TF2_1 , p2
#define TCODE_FUNCTION_TF2_3 TCODE_FUNCTION_TF2_2 , p3
#define TCODE_FUNCTION_TF2_4 TCODE_FUNCTION_TF2_3 , p4
#define TCODE_FUNCTION_TF2_5 TCODE_FUNCTION_TF2_4 , p5

    template < typename T >
    struct forward_traits {
        typedef const T& type;
    };

    template < typename T >
    struct forward_traits < const T > {
        typedef const T& type;
    };

    template < typename T >
    struct forward_traits < T& > {
        typedef T& type;
    };

    template < typename T >
    struct forward_traits < const T& > {
        typedef const T& type;
    };

    template < typename T >
    struct forward_traits < T* > {
        typedef T* type;
    };

    template < typename T >
    struct forward_traits < const T* > {
        typedef const T* type;
    };

    template < typename Handler >
    class function;

#define TCODE_FUNCTION_DECLARE_FUNCTION( TD , TS , TP , TF , TP2 , TF2 )\
    template < TD >\
    class function< TS > {\
        typedef R (*raw_handler_type)( TP ); \
        enum handler_ops { DUPLICATE , CLONE , RELEASE };\
        \
        template < typename Handler >\
        class handler_impl {\
        public:\
            handler_impl( const Handler& handler ) \
                : _handler( handler ){\
                _ref_count.store(1);\
            }\
            handler_impl( const handler_impl& rhs ) \
                : _handler( rhs._handler ){\
                _ref_count.store(1);\
            }\
            void release( void ) {\
                if ( _ref_count.fetch_sub( 1 , std::memory_order_release ) == 1 ) {\
                    delete this;\
                }\
            }\
            handler_impl* duplicate( void ) {\
                _ref_count.fetch_add(1);\
                return this;\
            }\
            handler_impl* clone( void ) {\
                return  new handler_impl( *this );\
            }\
            \
            R operator()( TP ){ \
                return _handler( TF ); \
            }\
            static R call( TP2 ) {\
                return (*reinterpret_cast<handler_impl*>(ctx))( TF );\
            }\
            static void* control( handler_ops op , void* ctx ){\
                handler_impl* impl = reinterpret_cast< handler_impl* >(ctx);\
                if ( op == handler_ops::DUPLICATE ) return impl->duplicate();\
                if ( op == handler_ops::CLONE) return impl->clone();\
                if ( op == handler_ops::RELEASE) impl->release();\
                return nullptr;\
            }\
        private:\
            std::atomic< int > _ref_count;\
            Handler _handler;\
        };\
    public:\
        function( raw_handler_type raw_handler ) \
            : _context( reinterpret_cast< void* >(raw_handler))\
            , _call( &function::function_ptr_call)\
            , _control( &function::function_ptr_control )\
        {\
        }\
        \
        template < typename Handler > \
        function( const Handler& handler ) {\
            _context = reinterpret_cast<void*>(\
                new handler_impl<Handler>(handler));\
            _call = handler_impl<Handler>::call;\
            _control = handler_impl<Handler>::control;  \
        }\
        \
        function( const function& rhs ){\
            _context = rhs._control( handler_ops::DUPLICATE , rhs._context );\
            _control= rhs._control;\
            _call = rhs._call;\
        } \
        \
        function( function&& rhs )\
            : _context( nullptr )\
            , _call( nullptr )\
            , _control( nullptr )\
        {\
            swap( rhs );\
        }\
        \
        function& operator=( raw_handler_type raw_handler ){\
            function f(raw_handler);\
            this->swap(f);\
            return *this;\
        }\
        \
        template < typename Handler >\
        function& operator=( const Handler& handler ){\
            function f(handler);\
            this->swap(f);\
            return *this;\
        }\
        \
        function& operator=( const function& rhs ) {\
            if ( _context == rhs._context )\
                return *this;\
            function f(std::move(*this));\
            _context = rhs._control( handler_ops::DUPLICATE , rhs._context );\
            _control= rhs._control;\
            _call = rhs._call;\
        }\
        \
        function& operator=( function&& rhs ) {\
            if ( _context == rhs._context )\
                return *this;\
            function f(std::move(*this));\
            swap(rhs);\
            return *this;\
        }\
        \
        ~function( void ) {\
            if ( _context && _control )\
                _control( handler_ops::RELEASE , _context);\
        }\
        \
        void swap( function& f ) {\
            std::swap( _context , f._context );\
            std::swap( _call , f._call);\
            std::swap( _control  , f._control );\
        }\
        \
        R operator()( TP ) {\
            return _call(TF2);\
        }\
        \
        static void* function_ptr_control( handler_ops op ,  void* ctx ) {\
            return ctx;\
        }\
        \
        static R function_ptr_call( TP2 ) {\
            return (reinterpret_cast<raw_handler_type>(ctx))(TF);\
        }\
    private:\
        function( void* c , R (*call)(TP2) , void* (*ctrl )( handler_ops , void* ))\
            : _context(c) , _call(call) , _control( ctrl ) {\
        }\
        function clone( void ) {\
            return function( _control( handler_ops::CLONE , _context ) , _call , _control);\
        }\
    private:\
        void* _context;\
        R (*_call)( TP2 );\
        void* (*_control)( handler_ops , void* );\
    };\



#define TCODE_FUNCTION_DECLARE_FUNCTION_PARAM_N( n ) \
		TCODE_FUNCTION_DECLARE_FUNCTION( \
		TCODE_FUNCTION_TD_##n , \
		TCODE_FUNCTION_TS_##n , \
		TCODE_FUNCTION_TP_##n , \
		TCODE_FUNCTION_TF_##n , \
        TCODE_FUNCTION_TP2_##n, \
        TCODE_FUNCTION_TF2_##n )
												
TCODE_FUNCTION_DECLARE_FUNCTION_PARAM_N(0)							
TCODE_FUNCTION_DECLARE_FUNCTION_PARAM_N(1)							
TCODE_FUNCTION_DECLARE_FUNCTION_PARAM_N(2)							
TCODE_FUNCTION_DECLARE_FUNCTION_PARAM_N(3)							
TCODE_FUNCTION_DECLARE_FUNCTION_PARAM_N(4)
TCODE_FUNCTION_DECLARE_FUNCTION_PARAM_N(5)

}

#endif
