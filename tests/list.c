#include "list.h"

#include <stdlib.h>

list* list_new()
{
    list* l = malloc(sizeof(list));
    l->first = NULL;
    l->size = 0;
    
    return l;
}

void list_delete(list* l)
{
    node* current = l->first;
    
    while (current)
    {
        node* next = current->next;
        free(current);
        
        current = next;
    }
    
    free(l);
}

int list_size(list* l)
{
    return l->size;
}

int list_append(list* l, value v)
{
    list_iterator it = list_iterator_begin(l);
    
    while (!list_iterator_end(it))
    {
        it = list_iterator_next(it);
    }
    
    return list_insert_before(it, v);
}

int list_remove(list* l, int idx)
{
    if (idx >= l->size)
        return 0;
    
    node* n = l->first;
    
    if (idx == 0)
    {
        l->first = n->next;
        free(n);
    }
    else
    {
        for (int i = 0; i < idx - 1; i++)
        {
            n = n->next;
        }
        
        node* rip = n->next;
        n->next = rip->next;
        
        free(rip);
    }
    
    l->size--;
    
    return 1;
}

list_iterator list_iterator_begin(list* l)
{
    list_iterator beg = {l, &l->first};
    return beg;
}

int list_iterator_end(list_iterator it)
{
    return *it.ptr == NULL;
}

list_iterator list_iterator_next(list_iterator it)
{
    list_iterator next = {it.l, &(*it.ptr)->next};
    return next;
}

int list_insert_before(list_iterator it, value v)
{
    node* n = malloc(sizeof(node));
    if (!n)
        return 0;
    
    n->v = v;
    n->next = *it.ptr;
    
    *it.ptr = n;
    
    it.l->size++;
    
    return 1;
}

int list_insert_after(list_iterator it, value v)
{
    node* n = malloc(sizeof(node));
    if (!n)
        return 0;
    
    n->v = v;
    
    n->next = (*it.ptr)->next;
    (*it.ptr)->next = n;
    
    it.l->size++;
    
    return 1;
}

value list_iterator_get(list_iterator it)
{
    return (*it.ptr)->v;
}
