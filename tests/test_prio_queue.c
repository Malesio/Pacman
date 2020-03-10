#include "priority_queue.h"

#include <stdio.h>

int compare(value l, value r)
{
    return l.weight - r.weight;
}

int main(int argc, char *argv[])
{
    priority_queue* q = priority_queue_new(compare);
    
    value init[] = {
        {0, 25},
        {1, 17},
        {2, 20},
        {3, 15},
        {4, 17},
        {5, 11}
    };
    
    for (int i = 0; i < 6; i++)
    {
        priority_queue_push(q, init[i]);
    }
    
    printf("Priority queue size: %d\n", priority_queue_size(q));
    
    while (priority_queue_size(q) > 0)
    {
        value v;
        priority_queue_top(q, &v);
        priority_queue_pop(q);
        
        printf("{%d: %d}\n", v.index, v.weight);
    }
    
    priority_queue_delete(q);
    
    return 0;
}
