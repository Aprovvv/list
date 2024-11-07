#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main()
{
    struct list_t* list = list_init(10);

    graph_dump(list);
    list_push_front(list, 10);
    graph_dump(list);
    list_push_front(list, 20);
    graph_dump(list);
    list_push(list, 0, 30);
    graph_dump(list);
    list_push(list, 0, 40);
    graph_dump(list);

    list_push_back(list, 10);
    graph_dump(list);
    list_push_back(list, 20);
    graph_dump(list);
    list_push_back(list, 30);
    graph_dump(list);
    list_push_back(list, 40);
    graph_dump(list);

    list_destroy(list);
}
