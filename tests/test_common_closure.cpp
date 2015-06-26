#include "stdafx.h"
#include <common/closure.hpp>

TEST( closure , make ) {
	typedef tcode::closure< void () > closure_type;
	closure_type::pointer_type ptr = closure_type::make_closure( [] {
			gprintf("Closer Called\n");
		});

	(*ptr)();

	// gcc 에선 vptr 에 8 바이트
	// t_handler 용으로 8 바이트 추가 하는 듯..
	gout << ptr->this_size() << gendl;

}

TEST( closure , return ) {
	typedef tcode::closure< int () > closure_type;
	closure_type::pointer_type ptr 
		= closure_type::make_closure([]() -> int{
			return 32;
		});
	gout << ptr->this_size() << gendl;
	ASSERT_EQ( (*ptr)() , 32 );

}

TEST( closure , param ) {
	typedef tcode::closure< int ( int ) > closure_type1;
	typedef tcode::closure< int ( int , int ) > closure_type2;
	closure_type1::pointer_type ptr
		= closure_type1::make_closure( [] ( int i ) -> int {
			return i;
		});
	ASSERT_EQ( (*ptr)(24) , 24 );
	gout << ptr->this_size() << gendl;
	closure_type2::pointer_type ptr2
		= closure_type2::make_closure( [] ( int x , int y ) -> int {
			return x + y;
		});
	ASSERT_EQ( (*ptr2)(12 , 12) , 24 );	
	gout << ptr2->this_size() << gendl;
}


template < typename sig >
class test_closure;


template < typename R >
class test_closure< R () >{
public:
	test_closure( void ) {

	}

	virtual ~test_closure( void ){

	}

private:
};


template < typename sig >
class test_closure2{
public:
	test_closure2( void ) {

	}

	~test_closure2( void ){

	}
};

template < typename sig >
class test_closure3{
public:
	test_closure3( void ) {

	}

	virtual ~test_closure3( void ){

	}
};

class test_closure4{
public:
	test_closure4( void ) {

	}

	virtual ~test_closure4( void ){

	}
};

class test_closure5 : public test_closure3< void () >{
public:
	test_closure5( void ) {

	}

	virtual ~test_closure5( void ){

	}
};


TEST( closuer , size ) {
	gout << "tet closure size " << sizeof(test_closure< void () >) << gendl;
	gout << "tet closure2 size " << sizeof(test_closure2< void () >) << gendl;
	gout << "tet closure3 size " << sizeof(test_closure3< void () >) << gendl;
	gout << "tet closure4 size " << sizeof(test_closure4) << gendl;
	gout << "tet closure5 size " << sizeof(test_closure5) << gendl;
}




void test_fn( void ) {
	gprintf("Handler Function Ptr \n");
}

TEST( handler , function_ptr ){

	tcode::handler< void () > handler(&test_fn);
	
	handler();

	//ASSERT_FALSE( handler.is_allocated() );
	
}

TEST( handler , lambda ){

	auto lambda = [] {
			gprintf("Handler Lambda \n");
		};
	tcode::handler< void () > handler( lambda );

	gprintf( "lambda size = %d closure = %d closure_size = %d \n"
				, sizeof( lambda )
				, handler.closure()->this_size()
				, sizeof( tcode::closure< void () >));
	
	handler();

	//ASSERT_FALSE( handler.is_allocated() );
	
}


TEST( handler , capture ){
	int value = 32;
	int value2 = 33;
	tcode::handler< void () > handler( [&value , &value2] {
		gprintf("Handler Capture %d %d \n" , value , value2);
	} );
	
	handler();

	ASSERT_TRUE( handler.is_allocated() );
	
}


TEST( handler , assign ){
	tcode::handler< void () > handler(&test_fn);
	tcode::handler< void () > handler2( handler );
	handler2();
	
	handler2 =[] {
		gprintf("Handler Lambda \n");
	};
	handler2();

	handler2 = handler;	
	handler2();
}

int test_fn_int( void ) {
	gprintf("Handler Function Ptr \n");
	return 0;
}

TEST( handler , int_ ){
	tcode::handler< int () > handler(&test_fn_int);
	
	ASSERT_EQ(handler(),0);

	//ASSERT_FALSE( handler.is_allocated() );
}


TEST( handler , int_param ){
	tcode::handler< int ( int) > handler(
		[]( int v ) -> int {
			return v;
		});
	
	ASSERT_EQ(handler(3),3);

	//ASSERT_FALSE( handler.is_allocated() );
}


TEST(handler, bind ){
	tcode::handler< int(int) > handler;

	handler.bind(
		[](int v) -> int {
		return v;
	});

	ASSERT_EQ(handler(3), 3);

	//ASSERT_FALSE(handler.is_allocated());
}
