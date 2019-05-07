#ifndef __ROOM__
#define __ROOM__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "LinkedList.h"

#include "Datastore.h"


#define NAME_MAX_LENGTH 24

typedef struct _room Room;

/**
 * @brief Structure to hold all data concerning a Room.
 * 
 */
struct _room {
    uint16_t id;
    Datastore* parentDatastore;
    list_element* listPtr;
    char* name;
    list* nodes;
    list* rules;
};

/**
 * @brief Create a Room object
 * 
 * @param datastore Datastore in which to be inserted.
 * @param id The ID for the new Room. Must be unique.
 * @return Room* The pointer to the new Room object. NULL if error occurs.
 */
Room* createRoom (Datastore* datastore, uint16_t id);

/**
 * @brief Delete a Room object and all it's childs
 * 
 * @param room The pointer to the Room object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteRoom (Room* room);

/**
 * @brief Set the Room ID object
 * 
 * @param room Pointer to the Room object
 * @param id 
 * @return true Error
 * @return false All good
 */
bool setRoomID (Room* room, uint16_t id);

/**
 * @brief Set the Room Name object
 * 
 * @param room Pointer to the Room object
 * @param str Room name
 * @return true Error
 * @return false All good
 */
bool setRoomName(Room* room, const char* str);

/**
 * @brief Searches the datastore for a Room with the specified roomID
 * 
 * @param datastore Datastore object to search
 * @param roomID roomID to find
 * @return Room* Room object with 'roomID' id. NULL if not found.
 */
Room* findRoomByID (Datastore* datastore, uint16_t roomID);

/**
 * @brief Searches the datastore for a Room with the specified roomName
 * 
 * @param datastore Datastore object to search
 * @param roomName roomName to find
 * @return Room* Room object with 'roomName' name. NULL if not found.
 */
Room* findRoomByName (Datastore* datastore, const char* roomName);

#endif
