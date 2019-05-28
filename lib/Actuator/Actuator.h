#ifndef __ACTUATOR__
#define __ACTUATOR__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LinkedList.h"

typedef struct _actuator Actuator;

#include "Pixel.h"
#include "Rule.h"
#include "Node.h"
#include "Position.h"


/**
 * @brief Structure to hold all data concerning an actuator.
 * 
 */
struct _actuator {
    Node* parentNode;
    list_element* listPtr;
    uint16_t id;
    uint8_t type;
    Pixel* pixel;
};

/**
 * @brief Create a Actuator object
 * 
 * @param node A pointer to the Node this Actuator will belong to.
 * @param id The ID of the new Actuator.
 * @param type Type of the Actuator
 * @param posX Position in the X axis on the RGB Matrix output
 * @param posY Position in the Y axis on the RGB Matrix output
 * @return Actuator* The pointer to the new Actuator object. NULL if error occurs.
 */
Actuator* createActuator (Node* node, uint16_t id, uint8_t type, Position* pos);

/**
 * @brief Delete a Actuator object
 * 
 * @param actuator The pointer to the Actuator object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteActuator (Actuator* actuator);

/**
 * @brief Get the Actuator Pixel object
 * 
 * @param actuator Pointer to the Actuator object
 * @return Pixel* Pointer to the Pixel object. NULL if error.
 */
Pixel* getActuatorPixel (Actuator* actuator);

/**
 * @brief Searches the datastore for a Actuator with the specified actuatorID
 * 
 * @param datastore Datastore object to search
 * @param actuatorID ID of the actuator to find
 * @return Actuator* Actuator object with 'actuatorID' id. NULL if not found.
 */
Actuator* findActuatorByID (Datastore* datastore, uint16_t actuatorID);

bool moveActuatorToNode (Actuator* actuator, Node* node, list* queryList);

void prepareActuatorQueries (list* queryList);
void preparePriorityActuatorQueries (list* queryList);

#endif