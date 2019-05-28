#include "Node.h"

Node* createNode (Room* room, uint16_t id) {
    if (!room) {
        return NULL;
    }

    Datastore* datastore = room->parentDatastore;
    if (findNodeByID(datastore, id)) {
        return NULL;
    }

    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        // Memory allocation failed
        return NULL;
    }

    list_element* elem = listInsert(room->nodes, node, NULL);
    if (elem == NULL) {
        // Insertion failed
        free(node);
        return NULL;
    }

    list* sensors = newList();
    if (sensors == NULL) {
        free(node);
        listRemove(room->nodes, elem);
        return NULL;
    }

    list* actuators = newList();
    if (actuators == NULL) {
        free(node);
        deleteList(sensors);
        listRemove(room->nodes, elem);
        return NULL;
    }

    node->id = id;
    node->parentRoom = room;
    node->listPtr = elem;
    node->sensors = sensors;
    node->actuators = actuators;

    return node;
}

bool deleteNode (Node* node) {
    if (node == NULL) {
        return 1;
    }

    list_element* elem = node->listPtr;
    Room* room = node->parentRoom;

    list_element* aux;


    // Delete all node's sensors
    aux = listStart(node->sensors);
    while (aux != NULL) {
        if (deleteSensor(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(node->sensors);
    }
    deleteList(node->sensors);

    // Delete all node's actuators
    aux = listStart(node->actuators);
    while (aux != NULL) {
        if (deleteActuator(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(node->actuators);
    }
    deleteList(node->actuators);

    free(node);
    list_element* res = listRemove(room->nodes, elem);
    if (res == NULL && listSize(room->nodes)) {
        return 1;
    }

    return 0;
}

bool setNodeID (Node* node, uint16_t id) {
    if (node == NULL) {
        return 1;
    }

    if (findNodeByID(node->parentRoom->parentDatastore, id)) {
        return 1;
    }

    node->id = id;

    return 0;
}

Node* findNodeByID (Datastore* datastore, uint16_t nodeID) {
    if (!datastore) {
        return NULL;
    }

    for (list_element* room_elem = listStart(datastore->rooms); room_elem != NULL; room_elem = room_elem->next) {
        Room* room = (Room*)room_elem->ptr;
        for (list_element* node_elem = listStart(room->nodes); node_elem != NULL; node_elem = node_elem->next) {
            Node* node = (Node*)node_elem->ptr;
            if (node->id == nodeID) {
                return node;
            }
        }
    }

    return NULL;
}

bool moveNodeToRoom (Node* node, Room* room, list* queryList) {
    if (!node || !room) {
        return true;
    }

    // REMOVE NODE FROM CURRENT ROOM
    DBQuery* query = findQueryByName(queryList, "remove_node_from_room");
    if (!query) {
        fprintf(stderr, "Error moving node.\n");
    }
    else {
        // query has been found
        char* params[query->nParams];
        for (int i = 0; i < query->nParams; i++) {
            params[i] = malloc(12*sizeof(char));
        }
        
        sprintf(params[0], "%d", node->parentRoom->id);
        sprintf(params[1], "%d", node->id);

        __DB_exec(
            query,
            params
        );

        for (int i = 0; i < query->nParams; i++) {
            free(params[i]);
        }
    }

    list_element* res = listRemove(node->parentRoom->nodes, node->listPtr);
    if (res == NULL && listSize(node->parentRoom->nodes)) {
        return true;
    }
    node->listPtr = NULL;

    // ADD NODE TO NEW ROOM
    query = findQueryByName(queryList, "add_node_to_room");
    if (!query) {
        fprintf(stderr, "Error moving node.\n");
    }
    else {
        // query has been found
        char* params[query->nParams];
        for (int i = 0; i < query->nParams; i++) {
            params[i] = malloc(12*sizeof(char));
        }
        
        sprintf(params[0], "%d" ,room->id);
        sprintf(params[1], "%d", node->id);

        __DB_exec(
            query,
            params
        );

        for (int i = 0; i < query->nParams; i++) {
            free(params[i]);
        }
    }

    list_element* elem = listInsert(room->nodes, node, NULL);
    node->listPtr = elem;
    node->parentRoom = room;

    return false;
}

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_node = {
    NULL,
    "create_table_node",
    "CREATE TABLE IF NOT EXISTS sinf.node("
    "node_id INTEGER NOT NULL PRIMARY KEY);",
    0
};

DBQuery create_node = {
    NULL,
    "create_node",
    "INSERT INTO sinf.node(node_id)"
    "VALUES($1);",
    1
};

DBQuery delete_node = {
    NULL,
    "delete_node",
    "DELETE FROM sinf.node WHERE node_id=$1;",
    1
};

void preparePriorityNodeQueries (list* queryList) {
    addQuerytoList(&create_table_node, queryList);
}

void prepareNodeQueries (list* queryList) {
    addQuerytoList(&create_node, queryList);
    addQuerytoList(&delete_node, queryList);
}
