#ifndef __tcode_slist_h__
#define __tcode_slist_h__

namespace tcode {

namespace slist{


#ifndef TCODE_SLIST_NODE_DECL
#define TCODE_SLIST_NODE_DECL( clazz )\
public:\
	clazz* slist_next( void );\
	clazz* slist_next( clazz* c );\
private:\
	clazz* _##clazz##_next;
#endif

#ifndef TCODE_SLIST_NODE_IMPL
#define TCODE_SLIST_NODE_IMPL( clazz )\
	clazz* clazz::slist_next( void ){\
		return _##clazz##_next;\
	}\
	clazz* clazz::slist_next( clazz* c ){\
		std::swap( _##clazz##_next , c );\
		return c;\
	}
#endif

//! 
//! \class	node
//! \brief	single linked list node
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 29
//! 
template < typename Object >
class node {
public:
	//! 생성자
	node( void ) 
		: _slist_next( nullptr){
	}

	//! 다음 노드
	Object* slist_next( void ) {
		return _slist_next;
	}

	//! 다음 노드 설정 
	//! 이전 값 리턴
	Object* slist_next( Object* nn ) {
		std::swap( _slist_next , nn );
		return nn;
	}
protected:
	//! 소멸자 node 로 소멸 금지
	~node( void ) {
	}
private:
	Object* _slist_next;
};

//! 
//! \class	queue
//! \brief	single linked list queue
//! \author	tk aoziczero@gmail.com
//! \date	2015 05 29
//! 
template < typename Object >
class queue {
public:
    explicit queue( Object* head = nullptr )
		: _head( head ) {
        Object* it = head;
        while ( it && ( it->slist_next() != nullptr ) ) {
            it = it->slist_next();
        }
        _tail = it;
    }

	queue( const queue& q ) 
		: _head( q._head ) , _tail( q._tail )
	{
	}

	queue( queue&& q ) 
		: _head( std::move(q._head) ) 
		, _tail( std::move(q._tail) )
	{		
		q._head = q._tail = nullptr;
	}

	queue& operator=( const queue& rhs ){
		_head = rhs._head;
		_tail = rhs._tail;
		return *this;
	}

	queue& operator=( queue&& q ){
		_head = std::move(q._head);
		_tail = std::move(q._tail);
		q._head = q._tail = nullptr;
		return *this;
	}

    ~queue( void ) {

	}

	void add_front( Object* node ) {
		if ( _head == nullptr ) {
			_head = _tail = node;
		} else {
			Object* head = _head;
			_head = node;
			_head->slist_next( head );
		}		
	}

    void add_tail( Object* node ) {
		if ( _head == nullptr ) {
			_head = _tail = node;
		} else {
			_tail->slist_next( node );
			_tail = node;
		}
	}

    Object*  front( void ) {
		return _head;
	}

    void pop_front( void ) {
		if ( _head ) {
			if ( _head == _tail ) {
				_head = _tail = nullptr;
			} else {
				Object* node = _head->slist_next(nullptr);
				_head = node;
			}
		} 
	}

    void add_tail( queue<Object>&& list ) {
		if ( !list.empty() ) {
			this->add_tail( list.front() );
			this->_tail = list._tail;
			list._head = list._tail = nullptr;
		}
	}

	bool empty( void ) const {
		return _head == nullptr;
	}

    int count( void ) const {
        int cnt = 0;
        Object* p = _head;
        while( p ) {
            ++cnt;
            p = p->slist_next();
        }
        return cnt;
    }
public:
/*
    class iterator {
    public:
        iterator( Object* node ) : _node( node ) {
        }
        ~iterator( void ) {
        }

        iterator operator++( int ) {
            iterator it( _node );
            if ( _node != nullptr ) 
                _node = _node->slist_next();
            return it;
        }

        iterator& operator++( void ) {
            if ( _node != nullptr ) 
                _node = _node->slist_next();
            return *this;
        }

        bool operator==( const iterator& rhs ) {
            return _node == rhs._node;
        }

        bool operator!=( const iterator& rhs ) {
            return _node != rhs._node;
        }

        bool operator==( const Object* node ) {
            return _node == node;
        }

        bool operator!=( const Object* node ) {
            return _node != node;
        }

        Object* operator->(void)const{
            return _node;
        }

        Object& operator*(void) const{
            return *_node;
        }
    private:
        Object* _node;
    };

    iterator begin()  {
        return iterator( _head );
    }

    iterator end() {
        return iterator( _tail->slist_next() );
    }
*/
private:
    Object* _head;
    Object* _tail;
};

}}


#endif
