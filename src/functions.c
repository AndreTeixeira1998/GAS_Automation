#include "functions.h"
#include "stdio.h"
#include <unistd.h>

void skipComments (FILE* file, char* line) {
    
    do {
        line = fgets(line, CONFIGURATION_FILE_MAX_LINE_SIZE, file);
    } while (!strncmp(line, "#", 1));

    return;
}

char* filterString (char* str) {
    if (!str) {
        return NULL;
    }

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '\r' || str[i] == '\n' || str[i] == '#') {
            str[i] = '\0';
            return str;
        }
    }

    return str;
}

Datastore* importConfiguration(const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    Datastore* datastore = createDatastore();
    if (!datastore) {
        return NULL;
    }
    char line[CONFIGURATION_FILE_MAX_LINE_SIZE];
    skipComments(file, line);

    // Get the number of rooms to read
    int nRooms = atoi(filterString(line));
    if (!nRooms) {
        deleteDatastore(datastore);
        return NULL;
    }

    // Read nRooms rooms
    for (int i = 0; i < nRooms; i++) {
        skipComments(file, line);

        // Parse line
        char* token = filterString(strtok(line, ","));
        Room* room = createRoom(datastore);
        if (!room) {
            deleteDatastore(datastore);
            return NULL;
        }

        if (setRoomID(room, atoi(token))) {
            deleteDatastore(datastore);
            return NULL;
        }

        token = filterString(strtok(NULL, ","));
        if (setRoomName(room, token)) {
            deleteDatastore(datastore);
            return NULL;
        }
    }

    skipComments(file, line);

    // Get the number of nodes to read
    int nNodes = atoi(filterString(line));
    if (!nNodes) {
        deleteDatastore(datastore);
        return NULL;
    }

    // Read nNodes nodes
    for (int i = 0; i < nNodes; i++) {
        skipComments(file, line);

        // Parse line
        char* token = filterString(strtok(line, ","));
        uint16_t nodeID = atoi(token);
        token = strtok(NULL, ",");
        uint16_t roomID = atoi(token);

        Room* room = findRoomByID(datastore, roomID);
        if (!room) {
            deleteDatastore(datastore);
            return NULL;
        }

        Node* node = createNode(room);
        if (!node) {
            deleteDatastore(datastore);
            return NULL;
        }

        if (setNodeID(node, nodeID)) {
            deleteDatastore(datastore);
            return NULL;
        }
    }

    return datastore;
}