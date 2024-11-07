#include <stdio.h>
#include <stdlib.h>
#include "list.h"

const elem_type POISON = -666;

static int find_free_cell (struct list_t* list);
static int index(struct list_t* list, int numb);

struct list_t* list_init (int start_capacity)
{
    struct list_t* list = (struct list_t*)calloc(sizeof(struct list_t), 1);
    list->ptr = (struct list_elem*)calloc(sizeof(struct list_elem), (size_t)start_capacity + 1);
    list->capacity = start_capacity;
    list->size = 0;
    list->tail = 0;
    list->head = 0;
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
    list_push(list, list->size, val);
    return 0;
}

int list_pop_back (struct list_t* list)
{
    if (list->size == 0)
    {
        fprintf(stderr, "EMPTY LIST\n");
        return 1;
    }
    /*list->ptr[list->head].next = - list->free;

    list->free = list->head;
    list->head = list->ptr[list->head].prev;

    list->ptr[list->free].val = POISON;
    list->ptr[list->head].next = 0;
    list->size--;*/

    return 0;
}

int list_push(struct list_t* list, int numb, elem_type val)
{
    if (list->size >= list->capacity)
    {
        fprintf(stderr, "BUFFER OVERFLOW\n");
        return 1;
    }

    int i = index(list, numb);
    int i_plus_1 = list->ptr[i].next;

    list->ptr[list->free].prev = i;
    /*fprintf(stderr, "i = %d\n", i);
    fprintf(stderr, "list->ptr[i].next = %d\n", list->ptr[i].next);*/
    list->ptr[i_plus_1].prev = list->free;
    list->ptr[i].next = list->free;
    //fprintf(stderr, "list->ptr[i].next = %d\n", list->ptr[i].next);

    list->free = - list->ptr[list->free].next;

    list->ptr[list->ptr[i].next].next = i_plus_1;
    list->ptr[list->ptr[i].next].val = val;

    fprintf(stderr, "list->ptr[i].next = %d\n", list->ptr[i].next);

    list->size++;
    return 0;
}

int list_push_front (struct list_t* list, elem_type val)
{
    return list_push(list, 0, val);
}

int list_pop_front (struct list_t* list)
{
    if (list->size == 0)
    {
        fprintf(stderr, "EMPTY LIST\n");
        return 1;
    }
    int prev_free = list->free;
    list->ptr[list->tail].prev = 0;
    list->ptr[list->tail].val = POISON;
    list->free = list->tail;
    list->tail = list->ptr[list->tail].next;
    list->ptr[list->free].next = - prev_free;
    list->size--;

    /*
    list->ptr[list->head].next = - list->free;

    list->free = list->head;
    list->head = list->ptr[list->head].prev;

    list->ptr[list->free].val = POISON;
    list->ptr[list->head].next = 0;
    list->size--;
    */
    return 0;
}

elem_type list_show (struct list_t* list, int numb)
{
    if (numb >= list->size)
        return 1;

    return list->ptr[index(list, numb)].val;
}

static int index(struct list_t* list, int numb)
{
    int index = 0;
    for (int i = 0; i < numb; i++)
        index = list->ptr[index].next;

    return index;
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
                    "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
                    "\t\t\t<TR><TD PORT=\"ip\">ip %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"next\">next %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"val\">val %d</TD></TR>\n"
                    "\t\t\t<TR><TD PORT=\"prev\">prev %d</TD></TR>\n",
                    i, i, list->ptr[i].next, list->ptr[i].val, list->ptr[i].prev);
        fprintf(fp, "\t\t</TABLE>>];\n");
        i = list->ptr[i].next;
        //fprintf(stderr, "i = %d\n", i);
    } while (i != 0);
    //fprintf(stderr, "\n");
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
    while (i <= list->capacity && i!= 0) {
        fprintf(fp, "\t\t%d [label = <\n"
                    "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">\n"
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
    /*fprintf(fp, "\n}\nnode[shape=rect];\n{\n");
    fprintf(fp, "{free -> %d; head -> %d; tail -> %d}\n}\n}", list->free, list->head, list->tail);*/
    fclose(fp);
    char command[100];
    sprintf(command, "dot -Tpng %s -o %s", dot_name, png_name);
    system(command);
}
