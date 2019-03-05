#include "stdlib.h"
#include "stdio.h"

#include "DataStructure.h"
#include "functions.h"

bool func1(Actuator* actuator) {
    printf("  %d\n", actuator->pos->x);
    return 0;
}

int main(int argc, char const *argv[])
{
    Datastore* datastore = importConfiguration("GASconfig.conf");
    if (!datastore) {
        printf("Error in config file.\n");
        return 1;
    }
    
    for (list_element* elem = listStart(datastore->rooms); elem != NULL; elem = elem->next) {
        Room* room = elem->ptr;
        printf("%d,%s\n", room->id, room->name);

        for (list_element* elem2 = listStart(room->nodes); elem2 != NULL; elem2 = elem2->next) {
            Node* node = elem2->ptr;
            printf("  %d\n", node->id);
        }
    }

    printf("Done\n");
    
    return 0;
}
