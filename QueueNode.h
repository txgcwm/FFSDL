#ifndef __QUEUENODE__H__
#define __QUEUENODE__H__

#include <stdio.h>

template<class T>
class QueueNode{
    private:
        T val;
        QueueNode<T> *next;
    public:
        QueueNode(T v);
        void setNext(QueueNode *next);
        void setVal(T val);
        QueueNode<T>* getNext();
        T getVal();
        ~QueueNode();
};

#endif

