/*Based on Jacob Sorber video "How to implement a queue in C"
  https://www.youtube.com/watch?v=FcIubL92gaI*/
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define INT_FLAG_TO_EMPTY -1
#define EMPTY_QUEUE INT_FLAG_TO_EMPTY


typedef struct qnode
{
  int value;
  struct qnode *next;

}node;

typedef struct queue
{
  node *head;
  node *tail;
}queue;


void init_queue(queue *q);
bool enqueue(queue *q, int value);
int dequeue(queue *q);

