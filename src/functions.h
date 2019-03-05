#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include "DataStructure.h"

#define CONFIGURATION_FILE_MAX_LINE_SIZE 100

/**
 * @brief Imports the configuration of the nodes and their layout from a file.
 * 
 * @param filename Filename and directory of the file to be read
 * @return Datastore* Datastore object with all the information already inserted. NULL if error.
 */
Datastore* importConfiguration(const char* filename);

#endif