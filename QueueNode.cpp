#include "QueueNode.h"

template<class T>
QueueNode<T>::QueueNode(T v) {
    this->val = v;
    this->next = NULL;
}

template<class T>
void QueueNode<T>::setNext(QueueNode<T> *next) {
    this->next = next;
}

template<class T>
QueueNode<T>* QueueNode<T>::getNext() {
    return next;
}

template<class T>
void QueueNode<T>::setVal(T v) {
    this->val = v;
}

template<class T>
T QueueNode<T>::getVal() {
    return val;
}

template<class T>
QueueNode<T>::~QueueNode() {
    this->next = NULL;
}
