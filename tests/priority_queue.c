#include "priority_queue.h"

#include <stdlib.h>

priority_queue* priority_queue_new(int(*cmp)(value, value))
{
    priority_queue* q = malloc(sizeof(priority_queue));
    q->l = list_new();
    q->cmp_func = cmp;
    
    return q;
}

void priority_queue_delete(priority_queue* q)
{
    list_delete(q->l);
    
    free(q);
}

int priority_queue_size(priority_queue* q)
{
    return list_size(q->l);
}

void priority_queue_push(priority_queue* q, value v)
{
    list_iterator it = list_iterator_begin(q->l);
    
    while (!list_iterator_end(it) 
        && q->cmp_func(list_iterator_get(it), v) < 0)
    {
        it = list_iterator_next(it);
    }
    
    list_insert_before(it, v);
}

int priority_queue_pop(priority_queue* q)
{
    if (priority_queue_size(q) == 0)
        return 0;
    
    return list_remove(q->l, 0);
}

int priority_queue_top(priority_queue* q, value* v)
{
    if (priority_queue_size(q) == 0)
        return 0;
    
    *v = list_iterator_get(list_iterator_begin(q->l));
    
    return 1;
}
