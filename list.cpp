#include <stdio.h>
#include <stdlib.h>
#include "list.h"

const elem_type POISON = -666;

static int find_free_cell (struct list_t* list);

struct list_t* list_init (int start_capacity)
{
    struct list_t* list = (struct list_t*)calloc(sizeof(struct list_t), 1);
    list->ptr = (struct list_elem*)calloc(sizeof(struct list_elem), (size_t)start_capacity);
    list->capacity = start_capacity;
    list->size = 1;
    list->tail = 0;
    list->head = 0;
    list->free = 1;

    list->ptr[0].val = 0;
    list->ptr[0].next = 0;
    for (int i = 1; i < (int)start_capacity; i++)
    {
        list->ptr[i].val = POISON;
        list->ptr[i].next = - (i + 1);
    }
    return list;
}

int list_destroy (struct list_t* list)
{
    free(list->ptr);
    free(list);
    return 0;
}

int list_push_back (struct list_t* list, elem_type val)
{
    if (list->size >= list->capacity)
    {
        fprintf(stderr, "BUFFER OVERFLOW\n");
        return 1;
    }

    list->ptr[list->head].next = list->free;
    list->ptr[list->free].prev = list->head;

    list->head = list->free;
    list->free = - list->ptr[list->free].next;

    list->ptr[list->head].val = val;
    list->ptr[list->head].next =  0;
    list->size++;

    return 0;
}

int list_pop_back (struct list_t* list)
{
    if (list->size == 1)
    {
        fprintf(stderr, "EMPTY LIST\n");
        return 1;
    }
    list->ptr[list->head].next = - list->free;

    list->free = list->head;
    list->head = list->ptr[list->head].prev;

    list->ptr[list->free].val = POISON;
    list->ptr[list->head].next = 0;
    list->size--;
    return 0;
}

int list_push_front (struct list_t* list, elem_type val)
{
    if (list->size >= list->capacity)
    {
        fprintf(stderr, "BUFFER OVERFLOW\n");
        return 1;
    }

    int next = list->ptr[list->free].next;

    list->ptr[list->tail].prev = list->free;
    list->ptr[list->free].next = list->tail;

    list->tail = list->free;
    list->free = - next;

    list->ptr[list->tail].val = val;
    list->ptr[list->tail].next =  0;
    list->size++;
    return 0;
}

int list_pop_front (struct list_t* list)
{
    if (list->size == 0)
    {
        fprintf(stderr, "EMPTY LIST\n");
        return 1;
    }
    //list->ptr[list->free].prev = - list->head;
    list->ptr[list->tail].next = - list->free;

    list->free = list->tail;
    list->tail = list->ptr[list->tail].next;

    list->ptr[list->free].val = POISON;
    list->ptr[list->tail].prev = 0;
    list->size--;
    return 0;
}

static int find_free_cell (struct list_t* list)
{
    for (int i = 0; i < list->capacity; i++)
    {
        if (list->ptr[i].val == POISON)
            return i;
    }
    return -1;
}

void text_dump (struct list_t* list)
{
    fprintf(stderr, "--------------------------------------------\n");
    fprintf(stderr, "capacity = %d ", list->capacity);
    fprintf(stderr, "size = %d ", list->size);
    fprintf(stderr, "free = %d\n", list->free);
    for (int i = 0; i < list->size; i++)
    {
        fprintf(stderr, "*%d: prev = %d; val = %d; next = %d\n",
                i, list->ptr[i].prev, list->ptr[i].val, list->ptr[i].next);
    }
    for (int i = list->size; i < list->capacity; i++)
    {
        fprintf(stderr, " %d: prev = %d; val = %d; next = %d\n",
                i, list->ptr[i].prev, list->ptr[i].val, list->ptr[i].next);
    }
    fprintf(stderr, "--------------------------------------------\n");
}

void graph_dump (struct list_t* list)
{
    static int numb = 0;
    numb++;
    char dot_name[100];
    char png_name[100];
    sprintf(dot_name, "dots/%d.dot", numb);
    sprintf(png_name, "pngs/%d.png", numb);
    //fprintf(stderr, "%s\n", dot_name);
    FILE* fp = fopen(dot_name, "w");
    //stderr = fp;
    fprintf(fp, //"<graph>\n"
                "digraph G{\n"
                "\tnode[shape=none; margin=0]\n"
                "\t{\n"
                "\t\trank=same\n");
    for (int i = 0; i < list->capacity; i++)
    {
        fprintf(fp, "\t\t%d [label = <\n"
                    "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
                    "\t\t\t<TR><TD PORT=\"ip\">ip %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"next\">next %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"val\">val %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"prev\">prev %d</TD></TR>\n",
                    i, i, list->ptr[i].next, list->ptr[i].val, list->ptr[i].prev);
        fprintf(fp, "\t\t</TABLE>>];\n");
    }
    fprintf(fp, "\t}\n");
    int i = list->tail;
    while (i != list->head)
    {
        fprintf(fp, "\t%d:<next> -> %d;\n", i, list->ptr[i].next);
        i = list->ptr[i].next;
    }
    while (i != list->tail)
    {
        fprintf(fp, "\t%d:<prev> -> %d;\n", i, list->ptr[i].prev);
        i = list->ptr[i].prev;
    }
    i = list->free;
    while ( - list->ptr[i].next < list->capacity)
    {
        //fprintf(stderr, "%d %d\n", i, list->free);
        fprintf(fp, "\t%d:<next> -> %d;\n", i, - list->ptr[i].next);
        i = - list->ptr[i].next;
    }
    fprintf(fp, "\nnode[shape=rect];\n");
    fprintf(fp, "{free -> %d; head -> %d; tail -> %d}\n}", list->free, list->head, list->tail);
    //fprintf(fp, "{rank=same; \"capacity %d\"; \"size %d\"}\n}", list->capacity, list->size);
    fclose(fp);
    char command[100];
    sprintf(command, "dot -Tpng %s -o %s", dot_name, png_name);
    system(command);
}
