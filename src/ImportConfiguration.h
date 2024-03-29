#ifndef __IMPORTCONFIGURATION_H__
#define __IMPORTCONFIGURATION_H__

#include "Datastore.h"
#include "Rule.h"
#include "Room.h"
#include "Node.h"
#include "Sensor.h"
#include "Actuator.h"
#include "cJSON.h"

/**
 * @brief Imports the configuration of the nodes and their layout from a file.
 * 
 * @param filename Filename and directory of the file to be read
 * @return Datastore* Datastore object with all the information already inserted. NULL if error.
 */
Datastore* importConfiguration(const char* filename);

#endif