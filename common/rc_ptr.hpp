#ifndef __tcode_rc_ptr_h__
#define __tcode_rc_ptr_h__

#include <atomic>

namespace tcode{

template < typename Object >
class rc_ptr;
//! 
//! \class	ref_counted
//! \brief	���� ī���� \n
//! rc_ptr �� ����ϴ� object �� ref_conted �� ��ӹ޾ƾ� �Ѵ�.
//! class target : public ref_counted< target >{...};
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 13
//! 
template < typename Object , typename Counter = std::atomic<int> >
class ref_counted {
protected:
	//! construct
	explicit ref_counted( void (*destructor)(Object* ) = nullptr)
		:_destructor(destructor)
	{
		_counter.store(0);
	}

	//! destruct
	~ref_counted(void){
	
	}
public:
	//! ������� ����
	int add_ref( void ){
		return _counter.fetch_add(1) + 1;
	}

	//! ������� ����
	int release( void ){
		//! �޸��庮���� release ������ operation �� �ٸ� thread ���� �����ǰ� �Ѵ�.
		int val = _counter.fetch_sub(1 
			, std::memory_order::memory_order_release ) - 1;
		if ( val == 0 ) {			
			Object* obj = static_cast<Object*>(this);
			if( _destructor ){
				_destructor( obj );
			} else {
				delete obj;
			}
		}
		return val;
	}
private:
	//! ī���� ����
	Counter _counter;	
	void (*_destructor)(Object* );
};


//! 
//! \class	rc_ptr
//! \brief	���� ī���� ������ \n
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 14
//! 
template < typename Object >
class rc_ptr {
public:	
	//! construct
    explicit rc_ptr( Object* p = nullptr ) 
        : _ptr( p ){
        add_ref();
    }

	//! construct
    rc_ptr( const rc_ptr& rhs ) 
        : _ptr( rhs.get()){
        add_ref();
    }

	//! construct
	template < class other_t >
    rc_ptr( const rc_ptr<other_t>& rhs ) 
        : _ptr(rhs.get()){
        add_ref();
    }
		
    rc_ptr& operator=( Object* t ) {
        rc_ptr np( t );
        swap( np );
        return *this;
    }

    rc_ptr& operator=( const rc_ptr& rhs ) {
        rc_ptr np( rhs );
        swap( np );
        return *this;
    }

	template < typename OtherT >
    rc_ptr& operator=( const rc_ptr<OtherT>& rhs ) {
        return this->operator=( rhs.get());
    }

	rc_ptr( rc_ptr&& rhs ) 
		: _ptr( rhs.get() ) {
		rhs._ptr = nullptr;
	}

	template < class other_t >
    rc_ptr( rc_ptr<other_t>&& rhs ) 
        : _ptr(rhs.get()){
        rhs._ptr = nullptr;
    }

	rc_ptr& operator=( rc_ptr&& rhs ) {
		std::swap( rhs._ptr , _ptr );
		rhs.release();
		rhs._ptr = nullptr;
		return *this;
    }

	template < typename OtherT >
    rc_ptr& operator=( rc_ptr<OtherT>&& rhs ) {
		rc_ptr cache( std::move(*this));
		std::swap(_ptr,rhs._ptr);
		return *this;
    }
	
    ~rc_ptr( void ) {
        release();
    }

    void release( void ) {
        if ( _ptr && ( _ptr->release() == 0 ))
			_ptr = nullptr;
    }

    void add_ref( void ) {
        if ( _ptr ) {
            _ptr->add_ref();
        }
    }

    Object* operator->(void) const {
        return get();
    }

    Object& operator*(void) const {
        return *get();
    }

    Object* get(void) const {
        return _ptr;
    }

    void attach( Object* t ) {
        release();
        _ptr = t;
    }

    Object* detach( void ) {
        Object* old = _ptr;
        _ptr = nullptr;
        return old;
    }

    void reset( Object* t = nullptr ) {
        rc_ptr new_ptr( t );
        swap( new_ptr );
    }

    void swap( rc_ptr& other ) {
        Object* t = other._ptr;
        other._ptr = _ptr;
        _ptr = t;
    }

    bool unique( void ) const {
        if ( _ptr ) {
            return _ptr->ref_count() == 1;
        }
        return false;
    }
/*
	typedef Object* ( rc_ptr<Object>::*unspecified_bool_type)() const;
	operator unspecified_bool_type( void ) const{
		return _ptr? &rc_ptr<Object>::::get : 0;
	}
*/
	explicit operator bool() const
	{	
		return (_ptr != 0);
	}
private:
    Object* _ptr;
};

template < typename T >
bool operator==( const rc_ptr<T>& lhs , const rc_ptr<T>& rhs ) {
    return lhs.get() == rhs.get();
}

template < typename T >
bool operator!=( const rc_ptr<T>& lhs , const rc_ptr<T>& rhs ) {
    return lhs.get() != rhs.get();
}



}

#endif