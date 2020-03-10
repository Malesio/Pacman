#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "list.h"

typedef struct
{
    list* l;
    int (*cmp_func)(value l, value r);
} priority_queue;

priority_queue* priority_queue_new(int(*cmp)(value, value));
void priority_queue_delete(priority_queue* q);

int priority_queue_size(priority_queue* q);

void priority_queue_push(priority_queue* q, value v);
int priority_queue_pop(priority_queue* q);

int priority_queue_top(priority_queue* q, value* v);


#endif // PRIORITY_QUEUE_H
