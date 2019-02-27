#include "stdlib.h"
#include "stdio.h"

#include "../lib/DataStructure/DataStructure.h"

int main(int argc, char const *argv[])
{
    Datastore* datastore = createDatastore();
    if (datastore == NULL)
        printf("Error");
    //printf("%x\n", datastore);

    Room* room1 = createRoom(datastore);
    if (room1 == NULL)
        printf("Error: room1");
    //printf("%x\n", room);

    Room* room2 = createRoom(room1->parentDatastore);
    if (room2 == NULL)
        printf("Error: room2");
    //printf("%x\n", room2);

    Node* node1 = createNode(room2);
    if (room2 == NULL)
        printf("Error: node1");
    
    return 0;
}
