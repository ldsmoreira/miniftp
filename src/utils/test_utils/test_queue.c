#include "../queue.h"

int main() {
  queue q1,q2,q3;

  init_queue(&q1);
  init_queue(&q2);
  init_queue(&q3);

  enqueue(&q1, 12);
  enqueue(&q2, 15);
  enqueue(&q3, 22);
  enqueue(&q1, 67);
  enqueue(&q2, 30);
  enqueue(&q3, 89);

  int test;

  while((test = dequeue(&q2)) != EMPTY_QUEUE){
    printf("test = %d\n", test);
  }
}