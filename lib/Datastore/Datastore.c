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

    list* rules = newList();
    if (rules == NULL) {
        deleteList(pixels);
        deleteList(rooms);
        free(datastore);
        return NULL;
    }

    list* profiles = newList();
    if (rules == NULL) {
        deleteList(rules);
        deleteList(pixels);
        deleteList(rooms);
        free(datastore);
        return NULL;
    }

    datastore->rooms = rooms;
    datastore->pixels = pixels;
    datastore->rules = rules;
    datastore->profiles = profiles;

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

    // Delete all datastore's profiles
    aux = listStart(datastore->profiles);
    while (aux != NULL) {
        if (deleteProfile(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(datastore->profiles);
    }
    deleteList(datastore->profiles);

    // Delete all datastore's rules
    aux = listStart(datastore->rules);
    while (aux != NULL) {
        if (deleteRule(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(datastore->rules);
    }
    deleteList(datastore->rules);


    free(datastore);

    return 0;
}
