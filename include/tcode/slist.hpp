#ifndef __tcode_slist_h__
#define __tcode_slist_h__

namespace tcode {

    class slist {
    public:
        template < typename Node >
        class queue {
        public:
            explicit queue( Node* head = nullptr )
                : _head ( head )
            {
                _tail = _head;
                while ( _tail->next() != nullptr )
                    _tail = tail->next();
            }

            queue( queue&& q ) 
                : _head( q._head ) , _tail( q._tail ){
                q._head = nullptr;
                q._tail = nullptr;
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
        private:
            Node* _head;
            Node* _tail;
        };

    };


}


#endif
