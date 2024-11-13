#ifndef LIST_H
#define LIST_H

typedef int elem_type;

struct list_node;
struct list_t;

struct list_t* list_init (size_t start_capacity);
int list_destroy (struct list_t* list);

list_node* list_insert(struct list_t* list, struct list_node* node, elem_type val);
list_node* list_push_front (struct list_t* list, elem_type val);
list_node* list_push_back (struct list_t* list, elem_type val);

list_node* list_pop (struct list_t* list, list_node* node);
list_node* list_pop_front (struct list_t* list);
list_node* list_pop_back (struct list_t* list);

void text_dump (struct list_t* list);
void graph_dump (const struct list_t* list);
elem_type get_val (list_node* node);

list_node* get_next (list_t* list, list_node* node);
list_node* get_prev (list_t* list, list_node* node);
list_node* get_start (struct list_t* list);
list_node* get_end (list_t* list);

#endif
