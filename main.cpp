#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main()
{
    struct list_t* list = list_init(6);

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

    list_destroy(list);
}
