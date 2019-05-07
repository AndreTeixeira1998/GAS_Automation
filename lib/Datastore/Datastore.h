#ifndef __DATASTORE__
#define __DATASTORE__

#include <stdlib.h>
#include <stdbool.h>

typedef struct _datastore Datastore;

#include "LinkedList.h"

#include "Room.h"
#include "Pixel.h"



/**
 * @brief Main structure that stores all data concerning a space.
 * 
 */
struct _datastore {
    list* rooms;
    list* pixels;
};

/**
 * @brief Create a Datastore object
 * 
 * @return Datastore* The pointer to the new Datastore object. NULL if error occurs.
 */
Datastore* createDatastore ();

/**
 * @brief Delete a Datastore object and all it's childs
 * 
 * @param datastore The pointer to the Datastore object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteDatastore (Datastore* datastore);

#endif