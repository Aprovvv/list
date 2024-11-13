#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main()
{
    struct list_t* list = list_init(11);
    text_dump(list);

    for (int i = 1; i <= 5; i++)
    {
        list_push_back(list, i*10);
        graph_dump(list);
    }

    for (int i = 1; i <=5; i++)
    {
        list_push_front(list, i*10);
        graph_dump(list);
    }

    list_node* p = get_start(list);
    p = get_next(list, p);
    p = get_next(list, p);
    p = get_next(list, p);
    list_insert(list, p, 52);
    graph_dump(list);
    list_insert(list, p, 69);
    text_dump(list);
    graph_dump(list);

    list_destroy(list);
}
