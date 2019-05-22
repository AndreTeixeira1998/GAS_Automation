#include "Room.h"

Room* createRoom (Datastore* datastore, uint16_t id) {
    if (!datastore || findRoomByID(datastore, id)) {
        return NULL;
    }

    Room* room = (Room*)malloc(sizeof(Room));
    if (room == NULL) {
        // Memory allocation failed
        return NULL;
    }

    list_element* elem = listInsert(datastore->rooms, room, NULL);
    if (elem == NULL) {
        // Insertion failed
        free(room);
        return NULL;
    }

    list* nodes = newList();
    if (nodes == NULL) {
        free(room);
        listRemove(datastore->rooms, elem);
        return NULL;
    }

    room->id = id;
    room->parentDatastore = datastore;
    room->listPtr = elem;
    room->name = NULL;
    room->nodes = nodes;

    return room;
}

bool deleteRoom (Room* room) {
    if (room == NULL) {
        return 1;
    }

    list_element* elem = room->listPtr;
    Datastore* datastore = room->parentDatastore;

    list_element* aux;

    // Delete all room's nodes
    aux = listStart(room->nodes);
    while (aux != NULL) {
        if (deleteNode(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(room->nodes);
    }
    deleteList(room->nodes);

    free(room->name);
    free(room);
    list_element* res = listRemove(datastore->rooms, elem);
    if (res == NULL && listSize(datastore->rooms)) {
        return 1;
    }

    return 0;
}

bool setRoomID (Room* room, uint16_t id) {
    if (room == NULL) {
        return 1;
    }

    if (findRoomByID(room->parentDatastore, id)) {
        return 1;
    }

    room->id = id;

    return 0;
}

bool setRoomName (Room* room, const char* str) {
    if (room == NULL || str == NULL) {
        return 1;
    }

    if (findRoomByName(room->parentDatastore, str)) {
        return 1;
    }

    room->name = (char*)realloc(room->name, strlen(str)+1);
    strcpy(room->name, str);

    return 0;
}

Room* findRoomByID (Datastore* datastore, uint16_t roomID) {
    if (!datastore) {
        return NULL;
    }

    for (list_element* room_elem = listStart(datastore->rooms); room_elem != NULL; room_elem = room_elem->next) {
        Room* room = (Room*)room_elem->ptr;
        if (room->id == roomID) {
            return room;
        }
    }

    return NULL;
}

Room* findRoomByName (Datastore* datastore, const char* roomName) {
    if (!datastore || !roomName) {
        return NULL;
    }
    for (list_element* room_elem = listStart(datastore->rooms); room_elem != NULL; room_elem = room_elem->next) {
        Room* room = (Room*)room_elem->ptr;
        if (!room->name) {
            continue;
        }
        if (!strcmp(room->name, roomName)) {
            return room;
        }
    }

    return NULL;
}

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_room = {
    NULL,
    "create_table_room",
    "CREATE TABLE IF NOT EXISTS sinf.room("
    "room_id INTEGER NOT NULL PRIMARY KEY,"
    "name CHAR(30) UNIQUE);",
    0
};

DBQuery create_room = {
    NULL,
    "create_room",
    "INSERT INTO sinf.room(room_id, name) "
    "VALUES($1, $2);",
    2
};

DBQuery delete_room = {
    NULL,
    "delete_room",
    "DELETE FROM sinf.room WHERE room_id=$1;",
    1
};

DBQuery create_table_room_node = {
    NULL,
    "create_table_room_node",
    "CREATE TABLE IF NOT EXISTS sinf.room_node("
    "room_node_id SERIAL NOT NULL PRIMARY KEY,"
    "node_id INTEGER NOT NULL,"
    "room_id INTEGER NOT NULL,"
    "start_date TIMESTAMP NOT NULL DEFAULT NOW(),"
    "end_date TIMESTAMP,"
    "FOREIGN KEY (node_id) REFERENCES sinf.node(node_id) ON UPDATE CASCADE ON DELETE CASCADE,"
    "FOREIGN KEY (room_id) REFERENCES sinf.room(room_id) ON UPDATE CASCADE ON DELETE CASCADE);",
    0
};

DBQuery add_node_to_room = {
    NULL,
    "add_node_to_room",
    "INSERT INTO sinf.room_node(room_id, node_id) "
    "VALUES($1, $2);",
    2
};

DBQuery remove_node_from_room = {
    NULL,
    "remove_node_from_room",
    "UPDATE sinf.room_node "
    "SET end_date = NOW() "
    "WHERE room_id = $1 AND node_id = $2 AND end_date = NULL;",
    2
};

void preparePriorityRoomQueries (list* queryList) {
    addQuerytoList(&create_table_room, queryList);
    addQuerytoList(&create_table_room_node, queryList);
}

void prepareRoomQueries (list* queryList) {
    addQuerytoList(&create_room, queryList);
    addQuerytoList(&delete_room, queryList);
    addQuerytoList(&add_node_to_room, queryList);
    addQuerytoList(&remove_node_from_room, queryList);
}
