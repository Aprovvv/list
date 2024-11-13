#include <stdio.h>
#include <stdlib.h>
#include "list.h"

//TODO: должен возвращать указатель на узел, который вставил.

struct list_node
{
    list_node* prev;
    elem_type val;
    list_node* next;
};

struct list_t
{
    list_node* ptr;
    size_t size;
    size_t capacity;
    list_node* free;
};

const elem_type POISON = -666;

static int resize (list_t* list, size_t new_capacity);

list_t* list_init (size_t start_capacity)
{
    list_t* list = (list_t*)calloc(sizeof(list_t), 1);
    list->ptr = (list_node*)calloc(sizeof(list_node), start_capacity + 1);
    if (list->ptr == NULL)
        return NULL;
    list->capacity = start_capacity;
    list->size = 0;
    list->free = list->ptr + 1;

    list->ptr[0].val = 0;
    list->ptr[0].next = list->ptr;
    list->ptr[0].prev = list->ptr;
    for (int i = 1; i <= (int)start_capacity; i++)
    {
        list->ptr[i].val = POISON;
        list->ptr[i].next = list->ptr + i + 1;
    }
    return list;
}

int list_destroy (list_t* list)
{
    free(list->ptr);
    free(list);
    return 0;
}

list_node* list_insert (list_t* list, list_node* node, elem_type val)
{
    if (list->size >= list->capacity)
    {
        if (resize(list, list->capacity*2 + 1) == 1)
            return NULL;
        list->capacity *= 2;
    }

    list_node* next_node = node->next;

    list->free->prev = node;
    next_node->prev = list->free;
    node->next = list->free;

    list->free = list->free->next;

    node->next->next = next_node;
    node->next->val = val;

    list->size++;
    return node->next;
}

list_node* list_pop (list_t* list, list_node* node)
{
    if (list->size == 0)
    {
        fprintf(stderr, "EMPTY LIST\n");
        return NULL;
    }
    list_node* i_plus_1 = node->next;
    list_node* i_minus_1 = node->prev;

    node->next = list->free;
    node->prev = NULL;
    node->val = POISON;
    list->free = node;

    i_plus_1->prev = i_minus_1;
    i_minus_1->next = i_plus_1;

    list->size--;
    return i_plus_1;
}

void text_dump (list_t* list)
{
    fprintf(stderr, "--------------------------------------------\n");
    fprintf(stderr, "capacity = %zu ", list->capacity);
    fprintf(stderr, "size = %zu ", list->size);
    fprintf(stderr, "free = %p\n", list->free);
    for (size_t i = 0; i <= list->capacity; i++)
    {
        fprintf(stderr, "%zu: prev = %p; val = %d; next = %p\n",
                i, list->ptr[i].prev, list->ptr[i].val, list->ptr[i].next);
    }
    fprintf(stderr, "--------------------------------------------\n");
}

void graph_dump (const list_t* list)
{
    static int numb = 0;
    numb++;
    if (numb == 1)
    {
        system("rm dots/*.dot");
        system("rm pngs/*.png");
    }
    char dot_name[20];
    char png_name[20];
    sprintf(dot_name, "dots/%d.dot", numb);
    sprintf(png_name, "pngs/%d.png", numb);
    FILE* fp = fopen(dot_name, "w");
    fprintf(fp, "digraph G{\n"
                "\tnode[shape=none; margin=0]\n"
                "\t{\n"
                "\t\trank=same\n");
    int i = 0;
    list_node* node = list->ptr;
    do {
        fprintf(fp, "\t\t%d [label = <\n"
                    "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                    "CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
                    "\t\t\t<TR><TD PORT=\"ip\">ip %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"addr\">addr %lX</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"next\">next %lX</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"val\">val %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"prev\">prev %lX</TD></TR>\n",
                    i, i, (unsigned long)node%10000,
                    (unsigned long)list->ptr[i].next%10000, list->ptr[i].val,
                    (unsigned long)list->ptr[i].prev%10000);
        fprintf(fp, "\t\t</TABLE>>];\n");
        i = (int)(node->next - list->ptr);
        node = node->next;
    } while (node != list->ptr);
    fprintf(fp, "\t}\n");

    i = 0;
    do {
        fprintf(fp, "\t%d:<next> -> %d;\n", i, (int)(list->ptr[i].next - list->ptr));
        i = (int)(list->ptr[i].next - list->ptr);
    } while (i != 0);

    i = 0;
    do {
        fprintf(fp, "\t%d:<prev> -> %d;\n", i, (int)(list->ptr[i].prev - list->ptr));
        i = (int)(list->ptr[i].next - list->ptr);
    } while (i != 0);

    fprintf(fp, "{\nrank=same;\n");
    i = (int)(list->free - list->ptr);
    node = list->free;
    while (i <= (int)list->capacity && i!= 0) {
        fprintf(fp, "\t\t%d [label = <\n"
                    "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                    "CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
                    "\t\t\t<TR><TD PORT=\"ip\">ip %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"addr\">addr %lX</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"next\">next %lX</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"val\">val %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"prev\">prev %lX</TD></TR>\n",
                    i, i, (unsigned long)node%10000,
                    (unsigned long)list->ptr[i].next%10000, list->ptr[i].val,
                    (unsigned long)list->ptr[i].prev%10000);
        fprintf(fp, "\t\t</TABLE>>];\n");
        fprintf(fp, "\t%d:<next> -> %d;\n", i, (int)(list->ptr[i].next - list->ptr));
        i = (int)(node->next - list->ptr);
        node = node->next;
    }
    fprintf(fp, "\t}\n}");
    fclose(fp);
    char command[55];
    sprintf(command, "dot -Tpng %s -o %s", dot_name, png_name);
    system(command);
}

elem_type get_val (list_node* node)
{
    return node->val;
}

list_node* get_next (list_node* node)
{
    return node->next;
}

list_node* get_prev (list_node* node)
{
    return node->prev;
}

list_node* get_start (list_t* list)
{
    return list->ptr[0].next;
}

list_node* get_end (list_t* list)
{
    return list->ptr[0].prev;
}

list_node* list_push_back (list_t* list, elem_type val)
{
    return list_insert(list, list->ptr[0].prev, val);
}

list_node* list_push_front (list_t* list, elem_type val)
{
    return list_insert(list, list->ptr, val);
}

list_node* list_pop_back (list_t* list)
{
    return list_pop(list, list->ptr[0].prev);
}

list_node* list_pop_front (list_t* list)
{
    return list_pop (list, list->ptr);
}

static int resize(list_t* list, size_t new_capacity)
{
    list_node* temp_ptr = list->ptr;
    temp_ptr = (list_node*)realloc(temp_ptr, new_capacity*sizeof(list_node));
    if (temp_ptr == NULL)
        return 1;
    list->ptr = temp_ptr;

    if (list->capacity > new_capacity)
        return 0;

    for (size_t i = list->capacity + 1; i < new_capacity; i++)
    {
        list->ptr[i].prev = 0;
        list->ptr[i].val = POISON;
        list->ptr[i].next = list->ptr + i;
    }

    return 0;
}
