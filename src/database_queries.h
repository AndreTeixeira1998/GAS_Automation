#ifndef __DATABASE_QUERIES__
#define __DATABASE_QUERIES__

#include "DBLink.h"

/////////////////////////////////
/////// QUERY DEFENITIONS ///////
/////////////////////////////////

#define QUERY_ARRAY_SIZE 2 // Change to match number of nodes below

DBQuery create_node = {
    NULL,
    "create_node",
    "INSERT INTO sinf.node "
    "DEFAULT VALUES;",
    0
};

DBQuery create_actuator = {
    NULL,
    "create_actuator",
    "INSERT INTO sinf.actuator(type, pixel_id) "
    "VALUES($1, $2);",
    2
};

DBQuery* queryArray[QUERY_ARRAY_SIZE] = {
    &create_node,
    &create_actuator
};

#endif