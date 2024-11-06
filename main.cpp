#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main()
{
    struct list_t* list = list_init(5);
    graph_dump(list);
    list_push_back(list, 52);
    graph_dump(list);
    list_push_back(list, 42);
    graph_dump(list);
    list_pop_back(list);
    graph_dump(list);
    list_pop_back(list);
    text_dump(list);
    graph_dump(list);

    list_pop_back(list);
    text_dump(list);
    graph_dump(list);

    list_destroy(list);
}
