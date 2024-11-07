#ifndef LIST_H
#define LIST_H

typedef int elem_type;

struct list_elem
{
    int prev;
    elem_type val;
    int next;
};

struct list_t
{
    struct list_elem* ptr;
    int size;
    int capacity;
    int head;
    int tail;
    int free;
};

struct list_t* list_init (int start_size);
int list_push_front (struct list_t* list, elem_type val);
int list_pop_front (struct list_t* list);
int list_push(struct list_t* list, int numb, elem_type val);
int list_destroy (struct list_t* list);
int list_push_back (struct list_t* list, elem_type val);
int list_pop_back (struct list_t* list);
void text_dump (struct list_t* list);
void graph_dump (const struct list_t* list);
elem_type list_show (struct list_t* list, int numb);

#endif
