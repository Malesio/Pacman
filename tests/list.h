#ifndef LIST_H
#define LIST_H

typedef struct
{
    int index;
    int weight;
} value;

typedef struct node
{
    value v;
    struct node* next;
} node;

typedef struct
{
    node* first;
    int size;
} list;

typedef struct
{
    list* l;
    node** ptr;
} list_iterator;

list* list_new();
void list_delete(list* l);

int list_size(list* l);

int list_append(list* l, value v);
int list_remove(list* l, int i);

list_iterator list_iterator_begin(list* l);
int list_iterator_end(list_iterator it);
list_iterator list_iterator_next(list_iterator it);

int list_insert_before(list_iterator it, value v);
int list_insert_after(list_iterator it, value v);

value list_iterator_get(list_iterator it);

#endif // LIST_H
