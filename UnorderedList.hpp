#ifndef UNORDERED_LIST_HPP
#define UNORDERED_LIST_HPP

#include "BaseList.hpp"

template <class T>
class UnorderedList : public BaseList<T> {
    using BaseList<T>::head;
    using BaseList<T>::tail;
    using BaseList<T>::elemCount;
    typedef typename BaseList<T>::Node Node;

public:
    void insert( const T& );
    void insert( size_t, const T& );
};

/*
 * Insert a copy of the data parameter
 * at the end of the list.
 */
template <class T>
void UnorderedList<T>::insert( const T& data )
{
    Node* node = new Node( data, NULL );

    if ( elemCount == 0 ) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    elemCount += 1;
}

/*
 * Insert a copy of the data parameter at
 * the specified position
 */
template <class T>
void UnorderedList<T>::insert( size_t pos, const T& data )
{
    // Insert at the end
    if ( elemCount == 0 || pos >= elemCount ) {
        insert( data );
        return;
    }

    if ( pos == 0 ) {
        head = new Node( data, head );
        elemCount += 1;
        return;
    }

    Node* prevNode = head;

    // Find the element at pos-1
    for ( size_t iter = 0; iter < pos - 1; iter += 1 ) {
        prevNode = prevNode->next;
    }

    prevNode->next = new Node( data, prevNode->next );
    elemCount += 1;        
}

#endif /* UNORDERED_LIST_HPP */
