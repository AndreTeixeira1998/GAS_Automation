#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>
#include "DataStructure.h"

typedef struct {
    PGconn* conn;
    char* name;
    char* query;
    int nParams;
} DBQuery;

char* getConnectionStringFromFile (const char* filename);
void DB_prepareAllSQLQueries (PGconn* conn);
Datastore* DB_importConfiguration (PGconn* conn);

/////////////////////////////////
/////// QUERY DEFENITIONS ///////
/////////////////////////////////

#define queryArraySize 2 // Change to match number of nodes below

DBQuery* queryArray[2] = {
    &create_node,
    &create_actuator
};

DBQuery create_node = {
    conn,
    "create_node",
    "INSERT INTO sinf.node "
    "DEFAULT VALUES;",
    0
}

DBQuery create_node = {
    conn,
    "create_actuator",
    "INSERT INTO sinf.actuator(type, pixel_id) "
    "VALUES($1, $2);",
    2
}

#endif