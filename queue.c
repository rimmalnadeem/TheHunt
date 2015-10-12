// queue.c ... simple Queue
// Written by John Shepherd, September 2015

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "queue.h"

typedef struct Node *Link;

typedef struct Node {
	Item val;
	Link  next;
} Node;
	
typedef struct QueueRep {
	Link  front;
	Link  back;
} QueueRep;

// Function signatures

Queue newQueue();
void disposeQueue(Queue);
void enterQueue(Queue,Item);
Item leaveQueue(Queue);
int  emptyQueue(Queue);
void showQueue(Queue q);
int dupCheck(Queue,Item);
int countQueue(Queue);

static Link newNode(Item);
static void disposeNode(Link);


// newQueue()
// - create an initially empty Queue
Queue newQueue()
{
	Queue new = malloc(sizeof(QueueRep));
	assert(new != NULL);
	new->front = NULL;
	new->back = NULL;
	return new;
}

// disposeQueue(Queue)
// - clean up memory associated with Queue
void disposeQueue(Queue q)
{
	if (q == NULL) return;
	Link next, curr = q->front;
	while (curr != NULL) {
		next = curr->next;
		disposeNode(curr);	
		curr = next;
	}
    free(q);

}

// enterQueue(Queue,Item)
// - add Str to back of Queue
void enterQueue(Queue q, Item it)
{
	Link new = newNode(it);
	if (q->front == NULL)
		q->front = q->back = new;
	else {
		// add into list of elems
		q->back->next = new;
		q->back = new;
	}
}

// leaveQueue(Queue)
// - return Item at front of Queue
Item leaveQueue(Queue q)
{
	assert (q->front != NULL);
    Item it = q->front->val;
	Link old = q->front;
	q->front = old->next;
	if (q->front == NULL)
		q->back = NULL;
	free(old);
	return it;
}

// emptyQueue(Queue)
// - check whether Queue is empty
int emptyQueue(Queue q)
{
	return (q->front == NULL);
}

// showQueue(Queue)
// - display Queue (for debugging)
void showQueue(Queue q)
{
	Link curr;
	if (q->front == NULL)
		printf("Queue is empty\n");
	else {
		printf("Queue (front-to-back):\n");
		int id = 0;
		curr = q->front;
		while (curr != NULL) {
			printf("[%03d] %d\n", id, curr->val);
			id++;
			curr = curr->next;
		}
	}
}

//int dupCheck(Queue)
//check whether an entry is already in the Queue
//return 0 if match
//return 1 for no match

int dupCheck(Queue q,Item it){
    
    Link curr;
    if (q->front == NULL)
        //printf("Queue is empty\n");
        return 1;
    else {
        //printf("Queue (front-to-back):\n");
        //int id = 0;
        curr = q->front;
        while (curr != NULL) {
            //printf("[%03d] %s\n", id, curr->val);
          //  id++;
            if(curr->val==it)
                return 0;
            curr = curr->next;
        }
        return 1;
    }
    
}

//int countQueue(Queue)
//return the number of nodes in the queue


int countQueue(Queue q){
    int count=0;
    Link curr;
    if (q->front == NULL)
        //printf("Queue is empty\n");
        return count;
    else {
        //printf("Queue (front-to-back):\n");
        //int id = 0;
        curr = q->front;
        while (curr != NULL) {
            count++;
            curr = curr->next;
        }
        return count;
    }
    
}




// Helper functions

static Link newNode(Item it)
{
	Link new = malloc(sizeof(Node));
	assert(new != NULL);
	new->val = it;
	new->next = NULL;
	return new;
}

static void disposeNode(Link curr)
{
	assert(curr != NULL);
//	free(curr->val);
	free(curr);
}

