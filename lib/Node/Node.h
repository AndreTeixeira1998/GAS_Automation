#ifndef __NODE__
#define __NODE__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LinkedList.h"

typedef struct _node Node;

#include "Datastore.h"
#include "Room.h"
#include "Sensor.h"
#include "Actuator.h"


/**
 * @brief Structure to hold all data concerning a Node.
 * 
 */
struct _node {
    uint16_t id;
    Room* parentRoom;
    list_element* listPtr;
    list* sensors;
    list* actuators;
};

/**
 * @brief Create a Node object
 * 
 * @param room A pointer to the Room this Node will belong to.
 * @param id The ID of the new Node. Must be unique.
 * @return Node* The pointer to the new Node object. NULL if error occurs.
 */
Node* createNode (Room* room, uint16_t id);

/**
 * @brief Delete a Node object and all it's childs
 * 
 * @param node The pointer to the Node object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteNode (Node* node);

/**
 * @brief Set the Node ID object
 * 
 * @param node Pointer to the Node object
 * @param id 
 * @return true Error
 * @return false All good
 */
bool setNodeID (Node* node, uint16_t id);

/**
 * @brief Searches the datastore for a Node with the specified nodeID
 * 
 * @param datastore Datastore object to search
 * @param nodeID nodeID to find
 * @return Node* Node object with 'nodeID' id. NULL if not found.
 */
Node* findNodeByID (Datastore* datastore, uint16_t nodeID);

bool moveNodeToRoom (Node* node, Room* room, list* queryList);

void prepareNodeQueries (list* queryList);
void preparePriorityNodeQueries (list* queryList);

#endif
