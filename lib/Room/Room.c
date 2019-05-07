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
