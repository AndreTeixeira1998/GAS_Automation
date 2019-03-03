#include "stdlib.h"
#include "stdio.h"

#include "DataStructure.h"

int main(int argc, char const *argv[])
{
    Datastore* datastore = createDatastore();
    if (datastore == NULL)
        printf("Error\n");
    //printf("%x\n", datastore);

    Room* room1 = createRoom(datastore);
    if (room1 == NULL)
        printf("Error: room1\n");
    //printf("%x\n", room);

    Room* room2 = createRoom(room1->parentDatastore);
    if (room2 == NULL)
        printf("Error: room2\n");
    //printf("%x\n", room2);

    Node* node1 = createNode(room1);
    if (node1 == NULL){
        printf("Error: node1\n");
    }
    if (setNodeID(node1, 1)) {
        printf("Error setNodeID(node1)\n");
    }

    Node* node2 = createNode(room2);
    if (node2 == NULL){
        printf("Error: node2\n");
    }
    if (!setNodeID(node2, 1)) { // supposed to throw error
        printf("Error setNodeID(node2)\n");
    }

    if (findNodeByID(datastore, 1) != node1) {
        printf("Error: findNodeByID(datastore, 1) != node1\n");
    }

    if (setRoomName(room1, "Bedroom")) {
        printf("Error: setRoomName(room1, \"Bedroom\")\n");
    }

    if (findRoomByName(datastore, "Bedroom") != room1) {
        printf("Error: findRoomByName(datastore, \"Bedroom\") != room1\n");
    }

    if (deleteDatastore(datastore)) {
        printf("Error: deleteDatastore(datastore)\n");
    }
    
    printf("Done\n");
    
    return 0;
}
