#ifndef __QUEUE__H__
#define __QUEUE__H__

#include "QueueNode.h"

template<class T>
class Queue
{
    private:
        QueueNode<T> *head;
        QueueNode<T> *tail;
        int size;
    public:
        Queue();
        bool push(QueueNode<T> *item);
        bool dequeue(QueueNode<T>* &item);
        int getSize();
};

#endif
