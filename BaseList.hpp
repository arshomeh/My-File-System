#ifndef LIST_HPP
#define	LIST_HPP

#include <stdlib.h>
#include "mylib.hpp"


template <class T>
class BaseList {
public:
    BaseList();
    BaseList( const BaseList& );
    virtual ~BaseList();

    bool empty() const;
    size_t size() const;
    T&   get( const size_t ) const;
    bool find(T&) const;
    void clear();
    void remove(T&);
    void erase( const size_t );

    virtual void insert( const T& ) = 0;

    friend void swap( BaseList<T>& x, BaseList<T>& y ) {
        mylib::swap( x.head, y.head );
        mylib::swap( x.tail, y.tail );
        mylib::swap( x.elemCount, y.elemCount );
    }

protected:
    struct Node {
        Node( const T&, Node* );
        virtual ~Node();

        T data;
        Node* next;
    };

    Node* head;
    Node* tail;
    size_t elemCount;
};


template <class T>
BaseList<T>::Node::Node( const T& d, Node* n )
    : data( d ), next( n )
{
}

template <class T>
BaseList<T>::Node::~Node()
{
}

template <class T>
BaseList<T>::BaseList()
    : head( NULL ), tail( NULL ), elemCount( 0 )
{
}

template <class T>
BaseList<T>::BaseList( const BaseList& other )
    : head( NULL ), tail( NULL ), elemCount( other.elemCount )
{
    if ( other.elemCount == 0 ) {
        return;
    }

    head = new Node( other.head->data, NULL );
    Node* temp = head;
    Node* otherTemp = other.head->next;

    while ( otherTemp != NULL ) {
        temp->next = new Node( otherTemp->data, NULL );
        temp = temp->next;
        otherTemp = otherTemp->next;
    }

    tail = temp;
}

template <class T>
BaseList<T>::~BaseList()
{
    Node* temp;

    while ( head != NULL ) {
        temp = head;
        head = head->next;
        delete temp;
    }
}

/*
 * Return true if the list is empty.
 */
template <class T>
bool BaseList<T>::empty() const
{
    return ( elemCount == 0 );
}

/*
 * Return the size/length of the list
 */
template <class T>
size_t BaseList<T>::size() const
{
    return elemCount;
}

/*
 * Return a reference of the i-th element (indexing starts from 0).
 */
template <class T>
T& BaseList<T>::get( size_t pos ) const
{
    Node* current = head;

    for ( size_t i = 0 ; i < pos ; ++i ) {
        current = current->next;
    }
    return current->data;
}

/*
 * Return true if there is an element in the list
 * the equals data parameter and false if not.
 */
template <class T>
bool BaseList<T>::find( T& data ) const
{
    if ( elemCount == 0 )
        return false;

    Node* currentNode = head;

    while ( currentNode != NULL ) {
        if ( currentNode->data == data ) {
            return true;
        }
        currentNode = currentNode->next;
    }
    return false;
}

/*
 * Erases all the elements of the list.
 */
template <class T>
void BaseList<T>::clear()
{
    Node* temp;

    while ( head != NULL ) {
        temp = head;
        head = head->next;
        delete temp;
    }
    head = tail = NULL;
    elemCount = 0;
}

/*
 * Remove all the elements that match the data parameter
 */
template <class T>
void BaseList<T>::remove( T& data )
{
    // Check the first element for removal, if true then repeat
    while ( head != NULL && head->data == data ) {
        Node* tempNode = head;

        head = head->next;
        delete tempNode;
        elemCount -= 1;
    }

    if ( elemCount <= 1 ) {
        tail = head;
        return;  // return if there are no more elements
    }

    // Check the rest of the list
    Node *currentNode = head->next;  // inspected node for removal
    Node *prevNode = head;           // the node before the inspected node

    do {
        if ( currentNode->data == data ) {
            prevNode->next = currentNode->next;
            delete currentNode;
            currentNode = prevNode->next;
            elemCount -= 1;
        } else {
            prevNode = currentNode;
            currentNode = currentNode->next;
        }
    } while ( currentNode != NULL );
    tail = prevNode;
}

/*
 * Remove the element at the position specified by the pos parameter
 */
template <class T>
void BaseList<T>::erase( size_t pos )
{
    if ( pos >= elemCount ) {
        return;
    }

    Node* current = head;

    if ( pos == 0 ) {
        // If there is only one element
        if ( elemCount == 1 ) {
            head = tail = NULL;
        } else {
            head = head->next;
        }
        delete current;        
    } else {
        // Find the element before the one that is being erased
        for ( size_t i = 0 ; i < pos - 1 ; ++i ) {
            current = current->next;
        }
        Node* toBeDeleted = current->next;
        current->next = current->next->next;

        // If the removed node is the last node fix the tail
        if ( pos == elemCount - 1 ) {
            tail = current;
        }
        delete toBeDeleted;
    }
    elemCount -= 1;
}

#endif	/* LIST_HPP */

