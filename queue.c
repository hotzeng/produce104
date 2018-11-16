/* queue.c */

#include "common.h"
#include "queue.h"
#include "printf.h"
#include "scheduler.h"

void queue_init(node_t * queue)
{
    queue->prev = queue->next = queue;
}

node_t *queue_get(node_t * queue)
{
    node_t *item;

    item = queue->next;
    if (item == queue) {
        /* The queue is empty */
        item = NULL;
    } else {
        /* Remove item from the queue */
        item->prev->next = item->next;
        item->next->prev = item->prev;
    }
    return item;
}

node_t *queue_remove(node_t * item) {
  item->prev->next = item->next;
  item->next->prev = item->prev;
}

int queue_size(node_t * queue)
{
  node_t *item;

  item = queue->next;
  if (item == queue) 
    return 0;

  int size = 1;

  while (item && item->next != queue) {
    size++;
    item = item->next;
  }

  return size;

}

void print_queue(node_t* queue) {
  node_t* item;
  item = queue->next;
  if (item == queue) {
    printf(2, 60, "empty queue!!!!!");
    return;
  }
  printf(3, 60, "print queue!!!!!");
  //if(!item) {
  //  printf(4, 60, "queue is empty!")
  //} 
 
  int i = 0;
  pcb_t * pitem;//= (pcb_t *) item;
  //printf(i, 60, "%d", pitem->pid);
  while(item != queue) {
    pitem = (pcb_t *) item; 
    printf(2+i++, 60, "%d, %d", pitem->pid, item);
    item = item->next;
  }
}

void queue_put(node_t * queue, node_t * item)
{
    item->prev = queue->prev;
    item->next = queue;
    item->prev->next = item;
    item->next->prev = item;
}

int queue_empty(node_t *queue)
{
  if( queue->next == queue )
    return 1;
  else
    return 0;
}

node_t *queue_first(node_t *queue)
{
  if( queue->next == queue )
    return NULL;
  else
    return queue->next;
}

void queue_put_sort(node_t *q, node_t *elt, node_lte lte)
{
  node_t *iter;
  for(iter = q->next; iter && iter != q; iter=iter->next)
  {
    if( lte(elt, iter) )
    {
      /* put elt before iter */
      queue_put(iter, elt);
      return;
    }
  }

  /* Either the queue is empty, or
   * /elt/ is larger than all other elements.
   * put it at the end of the queue.
   */
  queue_put(q, elt);
}

