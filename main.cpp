#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main()
{
    struct list_t* list = list_init(4);

    for (int i = 1; i <= 20; i++)
    {
        list_push_back(list, i*10);
        text_dump(list);
        graph_dump(list);
    }

    list_destroy(list);
}
