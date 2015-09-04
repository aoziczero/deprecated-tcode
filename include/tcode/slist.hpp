#ifndef __tcode_slist_h__
#define __tcode_slist_h__

namespace tcode {

    class slist {
    public:
        template < typename Type >
        class node {
        public:
            node( void )
                : _next(nullptr)
            {
            }

            Type*& next( void ){
                return _next;
            }
        protected:
            ~node( void ) {
            }
        private:
            Type* _next;
        };

        template < typename Node >
        class queue {
        public:
            explicit queue( Node* head = nullptr )
                : _head ( head )
            {
                _tail = _head;
                while ( _tail && (_tail->next() != nullptr ))
                    _tail = _tail->next();
            }

            queue( const queue& q )
                : _head( q._head ) , _tail( q._tail ){
            }

            queue& operator=( const queue& rhs ) {
                _head = rhs._head;
                _tail = rhs._tail;
                return *this;
            }

            queue( queue&& q ) 
                : _head( q._head ) , _tail( q._tail ){
                q._head = nullptr;
                q._tail = nullptr;
            }

            queue& operator=( queue&& q )
            {
                _head = q._head;
                _tail = q._tail;
                q._head = q._tail = nullptr;
            }

            void push_back( Node* n ){
                if ( _head == nullptr )
                    _head = _tail = n;
                else { 
                    _tail->next() = n;
                    _tail = n;
                }
                n->next() = nullptr;
            }

            void push_front( Node* n ){
                if ( _head == nullptr ) 
                    _head = _tail = n;
                else {
                    n->next() = _head;
                    _head = n;
                }
            }

            Node* front( void ) {
                return _head;
            }

            template < typename NodeEx >
            NodeEx* front(void){
                return static_cast< NodeEx* >(front());
            }

            void pop_front( void ){
                if ( _head == nullptr )
                    return;
                if ( _head == _tail )
                    _head = _tail = nullptr;
                else {
                    Node* node = _head->next();
                    std::swap( node , _head );
                    node->next() = nullptr;
                }
            }

            bool empty( void ) const {
                return _head == nullptr;
            }

            int size( void ) const {
                int sz = 0;
                Node* n = _head;
                while ( n ){
                    ++sz;
                    n = n->next();
                }
                return sz;
            }
            
            void splice( queue& q ) {
                if ( q.empty() )
                    return;
                if ( empty() ) {
                    _head = q._head;
                    _tail = q._tail;
                } else {
                    _tail->next() = q._head;
                    _tail = q._tail;
                }
                q._head = q._tail = nullptr;
            }
        private:
            Node* _head;
            Node* _tail;
        };

    };


}


#endif
