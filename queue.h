// queue.h ... interface to simple Queue
// Written by John Shepherd, September 2015

#ifndef QUEUE_H
#define QUEUE_H

#include "Places.h"

typedef struct QueueRep *Queue;
typedef LocationID Item;

// Function signatures

Queue newQueue();
void disposeQueue(Queue);
void enterQueue(Queue,Item);
Item leaveQueue(Queue);
int  emptyQueue(Queue);
void showQueue(Queue q);
int dupCheck(Queue, Item);
int countQueue(Queue);

#endif
