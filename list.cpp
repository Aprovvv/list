#include <stdio.h>
#include <stdlib.h>
#include "list.h"

const elem_type POISON = -666;

static int index(struct list_t* list, int numb);
static int resize(struct list_t* list, size_t new_capacity);

struct list_t* list_init (size_t start_capacity)
{
    struct list_t* list = (struct list_t*)calloc(sizeof(struct list_t), 1);
    list->ptr = (struct list_elem*)calloc(sizeof(struct list_elem), start_capacity + 1);
    list->capacity = start_capacity;
    list->size = 0;
    list->free = 1;

    list->ptr[0].val = 0;
    list->ptr[0].next = 0;
    for (int i = 1; i <= (int)start_capacity; i++)
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
    list_push(list, (int)list->size, val);
    return 0;
}

int list_push_front (struct list_t* list, elem_type val)
{
    return list_push(list, 0, val);
}

int list_pop_back (struct list_t* list)
{
    return list_pop(list, (int)list->size - 1);
}

int list_pop_front (struct list_t* list)
{
    return list_pop (list, 0);
}

int list_push(struct list_t* list, int numb, elem_type val)
{
    if (list->size >= list->capacity)
    {
        resize(list, list->capacity*2 + 1);
        list->capacity *= 2;
    }

    int i = index(list, numb);
    int i_plus_1 = list->ptr[i].next;

    list->ptr[list->free].prev = i;
    list->ptr[i_plus_1].prev = list->free;
    list->ptr[i].next = list->free;

    list->free = - list->ptr[list->free].next;

    list->ptr[list->ptr[i].next].next = i_plus_1;
    list->ptr[list->ptr[i].next].val = val;

    list->size++;
    return 0;
}

int list_pop (struct list_t* list, int numb)
{
    if (list->size == 0)
    {
        fprintf(stderr, "EMPTY LIST\n");
        return 1;
    }
    int i = list->ptr[index(list, numb)].next;
    int i_plus_1 = list->ptr[i].next;
    int i_minus_1 = list->ptr[i].prev;

    list->ptr[i].next = - list->free;
    list->ptr[i].prev = 0;
    list->ptr[i].val = POISON;
    list->free = i;

    list->ptr[i_plus_1].prev = i_minus_1;
    list->ptr[i_minus_1].next = i_plus_1;

    list->size--;
    return 0;
}

elem_type list_show (struct list_t* list, int numb)
{
    if (numb >= (int)list->size)
        return 1;

    return list->ptr[index(list, numb)].val;
}

void text_dump (struct list_t* list)
{
    fprintf(stderr, "--------------------------------------------\n");
    fprintf(stderr, "capacity = %zu ", list->capacity);
    fprintf(stderr, "size = %zu ", list->size);
    fprintf(stderr, "free = %d\n", list->free);
    for (size_t i = 0; i <= list->capacity; i++)
    {
        fprintf(stderr, "%zu: prev = %d; val = %d; next = %d\n",
                i, list->ptr[i].prev, list->ptr[i].val, list->ptr[i].next);
    }
    fprintf(stderr, "--------------------------------------------\n");
}

void graph_dump (const struct list_t* list)
{
    static int numb = 0;
    numb++;
    if (numb == 1)
    {
        system("rm dots/*.dot");
        system("rm pngs/*.png");
    }
    char dot_name[100];
    char png_name[100];
    sprintf(dot_name, "dots/%d.dot", numb);
    sprintf(png_name, "pngs/%d.png", numb);
    FILE* fp = fopen(dot_name, "w");
    fprintf(fp, "digraph G{\n"
                "\tnode[shape=none; margin=0]\n"
                "\t{\n"
                "\t\trank=same\n");
    int i = 0;
    do {
        fprintf(fp, "\t\t%d [label = <\n"
                    "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                    "CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
                    "\t\t\t<TR><TD PORT=\"ip\">ip %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"next\">next %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"val\">val %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"prev\">prev %d</TD></TR>\n",
                    i, i, list->ptr[i].next, list->ptr[i].val, list->ptr[i].prev);
        fprintf(fp, "\t\t</TABLE>>];\n");
        i = list->ptr[i].next;
    } while (i != 0);
    fprintf(fp, "\t}\n");

    i = 0;
    do {
        fprintf(fp, "\t%d:<next> -> %d;\n", i, list->ptr[i].next);
        i = list->ptr[i].next;
    } while (i != 0);

    i = 0;
    do {
        fprintf(fp, "\t%d:<prev> -> %d;\n", i, list->ptr[i].prev);
        i = list->ptr[i].prev;
    } while (i != 0);

    fprintf(fp, "{\nrank=same;\n");
    i = list->free;
    while (i <= (int)list->capacity && i!= 0) {
        fprintf(fp, "\t\t%d [label = <\n"
                    "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" "
                    "CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
                    "\t\t\t<TR><TD PORT=\"ip\">ip %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"next\">next %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"val\">val %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"prev\">prev %d</TD></TR>\n",
                    i, i, list->ptr[i].next, list->ptr[i].val, list->ptr[i].prev);
        fprintf(fp, "\t\t</TABLE>>];\n");
        fprintf(fp, "\t%d:<next> -> %d;\n", i, - list->ptr[i].next);
        i = - list->ptr[i].next;
    }
    fprintf(fp, "\t}\n}");
    fclose(fp);
    char command[100];
    sprintf(command, "dot -Tpng %s -o %s", dot_name, png_name);
    system(command);
}

static int index(struct list_t* list, int numb)
{
    int index = 0;
    for (int i = 0; i < numb; i++)
        index = list->ptr[index].next;

    return index;
}

static int resize(struct list_t* list, size_t new_capacity)
{
    fprintf(stderr, "realloc\n");
    list_elem* temp_ptr = list->ptr;
    temp_ptr = (list_elem*)realloc(temp_ptr, new_capacity*sizeof(list_elem));
    if (temp_ptr == NULL)
        return 1;
    list->ptr = temp_ptr;

    if (list->capacity > new_capacity)
        return 0;

    for (int i = list->capacity + 1; i < new_capacity; i++)
    {
        list->ptr[i].prev = 0;
        list->ptr[i].val = POISON;
        list->ptr[i].next = - (i + 1);
    }

    return 0;
}
