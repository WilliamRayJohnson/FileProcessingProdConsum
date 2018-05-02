#include <stdio.h>
#include <stdlib.h>

#include "q.h"

void dump_q(Q *q);

/* Initialize the Q. */
void init_q (Q *q)
{
  q->front = q->rear = NULL;
  q->size = 0;
}


/* Get the size of the queue */
unsigned int size (Q *q)
{
  return q->size;
}


/* Create a new node and attach it to the list. */
void enqueue (Q *q, void *e)
{
  NODE *newnode = (NODE *) malloc (sizeof (NODE));
  newnode->element = e;
  newnode->next = NULL;
  
  if (q->size == 0) {
    /* queue is empty */
    q->front = q->rear = newnode;
  } else {
    /* attach new node to front of list */
    q->rear->next = newnode;
    q->rear = newnode;
    /* This won't work on all compilers, since the first assignment may
       update q->rear before the second assignment evaluates its RHS.

    q->rear->next = q->rear = newnode;
    */
  }
  q->size++;
}

/* Remove element from rear of queue and return pointer to element it
   contains.  Results are undefined if called on an empty queue. */
void *dequeue (Q *q)
{
  if (q->size == 0) {
    fprintf (stderr, "Remove called on empty linked list queue.\n");
    return NULL;
  }

  NODE *n = q->front;
  q->front = n->next;
  void *el = n->element;

  q->size--;

  /* If queue is empty, clear rear. */
  if (q->size == 0) {
    q->rear = NULL;
  }
  
  free (n);
  return el;
}

void dump_q(Q *q)
{
  NODE *p = q->front;
  printf ("q contents:::\n");
  while (p != NULL) {
    printf ("%s\n", (char *) p->element);
    p = p->next;
  }
}
    





