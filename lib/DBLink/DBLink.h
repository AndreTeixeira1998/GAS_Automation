#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>
#include "Datastore.h"

typedef struct {
    PGconn* conn;
    char* name;
    char* query;
    int nParams;
} DBQuery;

char* getConnectionStringFromFile (const char* filename);
void DB_prepareAllSQLQueries (PGconn* conn, DBQuery** queryArray, uint queryArraySize);
Datastore* DB_importConfiguration (PGconn* conn);

#endif