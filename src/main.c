#include "stdlib.h"
#include "stdio.h"

#include "DataStructure.h"
#include "functions.h"

int main(int argc, char const *argv[])
{
<<<<<<< HEAD
    Datastore* datastore = createDatastore();
    if (datastore == NULL)
        printf("Error\n");
    //printf("%x\n", datastore);

    Room* room1 = createRoom(datastore, 0);
    if (room1 == NULL)
        printf("Error: room1\n");
    //printf("%x\n", room);

    Room* room2 = createRoom(room1->parentDatastore, 1);
    if (room2 == NULL)
        printf("Error: room2\n");
    //printf("%x\n", room2);

    Node* node1 = createNode(room1,1);
    if (node1 == NULL){
        printf("Error: node1\n");
    }

    Node* node2 = createNode(room2, 1);
    if (node2){
        printf("Error: node2\n");
    }

    if (findNodeByID(datastore, 1) != node1) {
        printf("Error: findNodeByID(datastore, 1) != node1\n");
    }

    if (setRoomName(room1, "Bedroom")) {
        printf("Error: setRoomName(room1, \"Bedroom\")\n");
=======
    Datastore* datastore = importConfiguration("GASconfig.conf");
    if (!datastore) {
        printf("Error in config file.\n");
        return 1;
>>>>>>> import of rooms and nodes done.
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
