//! 
//! \file	closure.hpp
//! 
#ifndef __tcode_closure_h__
#define __tcode_closure_h__

#include <common/lib_allocator.hpp>
#include <memory>

namespace tcode {

// 이전의 handler< TS > 와 같은 홀더보단.. pointer_type 형태로 드러내놓고 쓰자
// 복사 금지 / 소유권이전 / 새로 생성으로 사용..
// 복사등의 기능은 필요하면 붙이자..

// template declare
#define TCODE_CLOSURE_TD_0        typename R
#define TCODE_CLOSURE_TD_1 TCODE_CLOSURE_TD_0 , typename A1
#define TCODE_CLOSURE_TD_2 TCODE_CLOSURE_TD_1 , typename A2
#define TCODE_CLOSURE_TD_3 TCODE_CLOSURE_TD_2 , typename A3
#define TCODE_CLOSURE_TD_4 TCODE_CLOSURE_TD_3 , typename A4
#define TCODE_CLOSURE_TD_5 TCODE_CLOSURE_TD_4 , typename A5
						   
// template specialization parameter
#define TCODE_CLOSURE_TSP_0
#define TCODE_CLOSURE_TSP_1         A1
#define TCODE_CLOSURE_TSP_2 TCODE_CLOSURE_TSP_1 , A2
#define TCODE_CLOSURE_TSP_3 TCODE_CLOSURE_TSP_2 , A3
#define TCODE_CLOSURE_TSP_4 TCODE_CLOSURE_TSP_3 , A4
#define TCODE_CLOSURE_TSP_5 TCODE_CLOSURE_TSP_4 , A5
							
// template specialization	
#define TCODE_CLOSURE_TS_0 R ( TCODE_CLOSURE_TSP_0 )
#define TCODE_CLOSURE_TS_1 R ( TCODE_CLOSURE_TSP_1 )
#define TCODE_CLOSURE_TS_2 R ( TCODE_CLOSURE_TSP_2 )
#define TCODE_CLOSURE_TS_3 R ( TCODE_CLOSURE_TSP_3 )
#define TCODE_CLOSURE_TS_4 R ( TCODE_CLOSURE_TSP_4 )
#define TCODE_CLOSURE_TS_5 R ( TCODE_CLOSURE_TSP_5 )

// template parameter
#define TCODE_CLOSURE_TP_0
#define TCODE_CLOSURE_TP_1						typename forward_traits< A1 >::type p1 
#define TCODE_CLOSURE_TP_2 TCODE_CLOSURE_TP_1 , typename forward_traits< A2 >::type p2
#define TCODE_CLOSURE_TP_3 TCODE_CLOSURE_TP_2 , typename forward_traits< A3 >::type p3
#define TCODE_CLOSURE_TP_4 TCODE_CLOSURE_TP_3 , typename forward_traits< A4 >::type p4
#define TCODE_CLOSURE_TP_5 TCODE_CLOSURE_TP_4 , typename forward_traits< A5 >::type p5

// template forward
#define TCODE_CLOSURE_TF_0
#define TCODE_CLOSURE_TF_1						p1
#define TCODE_CLOSURE_TF_2 TCODE_CLOSURE_TF_1 , p2
#define TCODE_CLOSURE_TF_3 TCODE_CLOSURE_TF_2 , p3
#define TCODE_CLOSURE_TF_4 TCODE_CLOSURE_TF_3 , p4
#define TCODE_CLOSURE_TF_5 TCODE_CLOSURE_TF_4 , p5


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

		
//! 
//! \class	closure
//! \brief  호출 가능한 오브젝트의 일반화 템플릿의 실구현\n
//! 할당해서 사용가능하며 handler<T> 로 warpping 해서 사용할것
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 28
//! 		   
template < typename signature >
class closure;

//! 특수화별 매크로!
#define TCODE_CLOSURE_DECLARE_CLOSURE( TD , TS , TP , TF )							\
template < TD >																		\
class closure< TS >  : public tcode::object_allocator< closure< TS > > {			\
public:																				\
	closure( void ){}																\
	virtual ~closure( void ){}														\
	virtual closure* copy( void* ptr , const std::size_t sz ) = 0;					\
	virtual std::size_t this_size( void ) const = 0;								\
	virtual R operator()( TP ) = 0;													\
	DISALLOW_COPY( closure< TS > )													\
	typedef std::shared_ptr< closure< TS > > pointer_type;							\
																					\
	template < typename t_lambda >													\
	class closure_impl ;															\
																					\
	template < typename t_lambda >													\
	static closure* bind( const t_lambda& lambda , void* ptr 						\
			, const std::size_t sz  ){												\
		if (ptr==nullptr || sz < sizeof(closure_impl<t_lambda>)){					\
			return new closure_impl<t_lambda>(lambda);								\
		}																			\
		new ( ptr ) closure_impl<t_lambda>(lambda);									\
		return static_cast< closure_impl<t_lambda>* >( ptr );						\
	}																				\
																					\
	template < typename t_lambda >													\
	static pointer_type make_closure( const t_lambda& lambda ) {					\
		pointer_type ptr(new closure_impl<t_lambda>(lambda));						\
		return ptr;																	\
	}																				\
};																					\
template < TD >																		\
template < typename t_lambda >														\
class closure< TS >::closure_impl : public closure< TS > {							\
public:																				\
	closure_impl( const t_lambda& l ): _handler(l) {}								\
	virtual ~closure_impl( void ) {}												\
	virtual std::size_t this_size( void ) const {									\
		return sizeof(*this);														\
	}																				\
	virtual closure* copy( void* ptr , const std::size_t sz ){						\
		if (ptr==nullptr || sz < sizeof(*this)){									\
			return new closure_impl(_handler);										\
		}																			\
		new ( ptr ) closure_impl(_handler);											\
		return static_cast< closure_impl* >( ptr );									\
	}																				\
	virtual R operator()( TP ) {													\
		return _handler(TF);														\
	}																				\
private:																			\
	t_lambda _handler;																\
};																					\

#define TCODE_CLOSURE_DECLARE_CLOSURE_PARAM_N( n ) \
		TCODE_CLOSURE_DECLARE_CLOSURE( \
		TCODE_CLOSURE_TD_##n , \
		TCODE_CLOSURE_TS_##n , \
		TCODE_CLOSURE_TP_##n , \
		TCODE_CLOSURE_TF_##n )
												
TCODE_CLOSURE_DECLARE_CLOSURE_PARAM_N(0)							
TCODE_CLOSURE_DECLARE_CLOSURE_PARAM_N(1)							
TCODE_CLOSURE_DECLARE_CLOSURE_PARAM_N(2)							
TCODE_CLOSURE_DECLARE_CLOSURE_PARAM_N(3)							
TCODE_CLOSURE_DECLARE_CLOSURE_PARAM_N(4)
TCODE_CLOSURE_DECLARE_CLOSURE_PARAM_N(5)




//! 
//! \class	handler
//! \brief  호출 가능한 오브젝트의 일반화 템플릿 \n
//! 추가 공간을 사용하여 object 크기가 8byte 이내라면 할당 오버헤드가 없다.
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 28
//! 	
template < typename Signature >
class handler;
																								
//! 특수화별 매크로
#define TCODE_HANDLER_DECLARE_HANDLER( TD , TS , TP , TF )										\
template < TD >																					\
class handler< TS > {																			\
public:																							\
	typedef tcode::closure< TS > closure_type;													\
public:																							\
	handler( void ) 																			\
		: _closure( nullptr ) , _buffer(0){														\
	}																							\
																								\
	handler( const handler& rhs ) 																\
		: _closure( rhs.copy( &_buffer , sizeof(_buffer)))										\
	{																							\
	}																							\
																								\
	template < typename t_handler >																\
	handler( const t_handler& handler )															\
		: _closure( closure_type::bind( handler , &_buffer , sizeof( _buffer ))){				\
	}																							\
																								\
	handler& operator=( const handler& rhs ) {													\
		if ( rhs._closure != _closure ){														\
			cleanup();																			\
			_closure = rhs.copy( &_buffer , sizeof( _buffer));									\
		}																						\
		return *this;																			\
	}																							\
																								\
	template < typename t_handler >																\
	handler& operator=( const t_handler& handler ) {											\
		cleanup();																				\
		_closure = closure_type::bind( handler , &_buffer , sizeof( _buffer ));					\
		return *this;																			\
	}																							\
																								\
	template < typename t_handler >																\
	void bind(const t_handler& handler) {														\
		cleanup();																				\
		_closure = closure_type::bind(handler, &_buffer, sizeof(_buffer));						\
	}																							\
																								\
	~handler( void ) {																			\
		cleanup();																				\
	}																							\
																								\
	R operator() ( TP ) {																		\
		return _closure? (*_closure)( TF ) : R();												\
	}																							\
																								\
	bool is_allocated( void ) const {															\
		return _closure != reinterpret_cast<closure_type*>(const_cast<int64_t*>(&_buffer));		\
	}																							\
																								\
	explicit operator bool(void) const {														\
		return (_closure != 0);																	\
	}																							\
	closure_type* closure(void) const{															\
		return _closure;																		\
	}																							\
private:																						\
	void cleanup( void ) {																		\
		if ( _closure == nullptr )																\
			return;																				\
		if ( is_allocated() ) {																	\
			delete _closure;																	\
		} else {																				\
			_closure->~closure_type();															\
		}																						\
		_closure = nullptr;																		\
		_buffer = 0;																			\
	}																							\
	closure_type* copy( void* ptr , const std::size_t sz ) const {								\
		return _closure ? _closure->copy( ptr ,sz ) : nullptr;									\
	}																							\
private:																						\
	closure_type* _closure;																		\
	int64_t _buffer;																			\
};																								\

#define TCODE_HANDLER_DECLARE_HANDLER_PARAM_N( n ) \
		TCODE_HANDLER_DECLARE_HANDLER( \
		TCODE_CLOSURE_TD_##n , \
		TCODE_CLOSURE_TS_##n , \
		TCODE_CLOSURE_TP_##n , \
		TCODE_CLOSURE_TF_##n )
												
TCODE_HANDLER_DECLARE_HANDLER_PARAM_N(0)							
TCODE_HANDLER_DECLARE_HANDLER_PARAM_N(1)							
TCODE_HANDLER_DECLARE_HANDLER_PARAM_N(2)							
TCODE_HANDLER_DECLARE_HANDLER_PARAM_N(3)							
TCODE_HANDLER_DECLARE_HANDLER_PARAM_N(4)
TCODE_HANDLER_DECLARE_HANDLER_PARAM_N(5)

/* 노가다성 이전처럼 걍 매크로로..
template < typename R >
class closure< R () >  : public ref_counted< closure< R () > > {
public:
	closure( void ){}
	virtual ~closure( void ){}
	virtual R operator()( void ) = 0;
	
	typedef typename closure< R () > this_type;	
	typedef rc_ptr< this_type > pointer_type;

	template < typename t_lambda >
	class closure_impl : public this_type {
	public:
		closure_impl( t_lambda& l ): _handler(l) {}
		virtual ~closure_impl( void ) {}
		virtual R operator()( void ) {
			return _handler();
		}
	private:
		t_lambda _handler;
	};

	template < typename t_lambda >
	static pointer_type make_closure( t_lambda& lambda ) {
		closure_impl<t_lambda>* p = new closure_impl<t_lambda>(lambda);
		pointer_type ptr(p);
		return ptr;
	}
};


template < typename R , typename A1 >
class closure< R ( A1 ) >  : public ref_counted< closure< R ( A1 ) > > {
public:
	closure( void ){}
	virtual ~closure( void ){}
	virtual R operator()(
		typename tcode::tmp::forward_traits< A1 >::type p1
	) = 0;
	
	typedef typename closure< R ( A1 ) > this_type;	
	typedef rc_ptr< this_type > pointer_type;

	template < typename t_lambda >
	class closure_impl : public this_type {
	public:
		closure_impl( t_lambda& l ) : _handler(l){}
		virtual ~closure_impl( void ) {}

		virtual R operator()( 
			typename tcode::tmp::forward_traits< A1 >::type p1
		) {
			return _handler(p1);
		}
	private:
		t_lambda _handler;
	};

	template < typename t_lambda >
	static pointer_type make_closure( t_lambda& lambda ) {
		closure_impl<t_lambda>* p = new closure_impl<t_lambda>(lambda);
		pointer_type ptr(p);
		return ptr;
	}
};


template < typename R , typename A1 , typename A2 >
class closure< R ( A1 , A2 ) >  : public ref_counted< closure< R ( A1, A2 ) > > {
public:
	closure( void ){}
	virtual ~closure( void ){}
	virtual R operator()(
		typename tcode::tmp::forward_traits< A1 >::type p1
	  , typename tcode::tmp::forward_traits< A2 >::type p2
	) = 0;
	
	typedef typename closure< R ( A1, A2 ) > this_type;	
	typedef rc_ptr< this_type > pointer_type;

	template < typename t_lambda >
	class closure_impl : public this_type {
	public:
		closure_impl( t_lambda& l ) : _handler(l){}
		virtual ~closure_impl( void ) {}

		virtual R operator()( 
			typename tcode::tmp::forward_traits< A1 >::type p1
		  , typename tcode::tmp::forward_traits< A2 >::type p2
		) {
			return _handler(p1 , p2);
		}
	private:
		t_lambda _handler;
	};

	template < typename t_lambda >
	static pointer_type make_closure( t_lambda& lambda ) {
		closure_impl<t_lambda>* p = new closure_impl<t_lambda>(lambda);
		pointer_type ptr(p);
		return ptr;
	}
};
*/



/*
//! 리턴값만 처리하게 하려 햇으나..
//! 파라미터도 추가 가능하게 수정..
template < typename R >
class closure : public ref_counted< closure<R> > {
public:
	closure( void ){}
	virtual ~closure( void ){}
	virtual R operator()( void ) = 0; 
};

template < typename R >
using closure_ptr = tcode::rc_ptr< closure<R> >;

template < typename R , typename T >
class closure_impl : public closure< R > {
public:
	closure_impl( T& t )
		: _handler(t)
	{
	}

	virtual ~closure_impl( void ) {
	}

	virtual R operator()( void ) {
		return _handler();
	}
private:
	T _handler;
};

template < typename t_lambda >
auto make_closure( t_lambda& lambda ) -> closure_ptr< decltype( lambda() ) > 
{	
	typedef decltype( lambda() ) ResultType;	
	closure< ResultType >* ptr = new closure_impl< ResultType , t_lambda >(lambda);
	closure_ptr< ResultType > p( ptr );
	return p;
}
*/

}

#endif
