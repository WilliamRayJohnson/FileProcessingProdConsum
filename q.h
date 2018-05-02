#ifndef Q_H
#define Q_H

/* Defintions to implement a generic FIFO queue using using a linked
   list. */

/* The queue is an ordered list of nodes. */
struct node {
  void *element;
  struct node *next;
};

typedef struct node NODE;

struct linked_list_queue {
  /* rear points to last item added.  front points to next item to
     remove. */
  NODE *front, *rear;
  unsigned int size;
};

typedef struct linked_list_queue Q;

/* Initialize the queue. */
void init_q (Q *q);

/* Get the size of the queue. */
unsigned int size (Q *q);

/* Add element e to the queue */
void enqueue (Q *q, void *e);

/* Remove the front element from the queue.  Results are undefined if
   the queue is empty. */
void *dequeue(Q *q);

#endif
