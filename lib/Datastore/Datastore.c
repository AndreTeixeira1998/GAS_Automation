#include "Datastore.h"


Datastore* createDatastore () {
    Datastore* datastore = (Datastore*)malloc(sizeof(Datastore));
    if (datastore == NULL) {
        // Memory allocation failed
        return NULL;
    }

    list* rooms = newList();
    if (rooms == NULL) {
        // Error creating list
        free(datastore);
        return NULL;
    }

    list* pixels = newList();
    if (pixels == NULL) {
        // Error creating list
        deleteList(rooms);
        free(datastore);
        return NULL;
    }

    datastore->rooms = rooms;
    datastore->pixels = pixels;

    return datastore;
}

bool deleteDatastore (Datastore* datastore) {
    if (datastore == NULL) {
        return 1;
    }

    list_element* aux;

    // Delete all datastore's rooms
    aux = listStart(datastore->rooms);
    while (aux != NULL) {
        if (deleteRoom(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(datastore->rooms);
    }
    deleteList(datastore->rooms);

    // Delete all datastore's pixels
    aux = listStart(datastore->pixels);
    while (aux != NULL) {
        if (deletePixel(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(datastore->pixels);
    }
    deleteList(datastore->pixels);

    free(datastore);

    return 0;
}
