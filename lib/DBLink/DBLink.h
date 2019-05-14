#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>

typedef struct _dbquery DBQuery;

#include "Datastore.h"

struct _dbquery {
    PGconn* conn;
    char* name;
    char* query;
    int nParams;
};

char* getConnectionStringFromFile (const char* filename);
void DB_prepareAllSQLQueries (PGconn* conn, DBQuery** queryArray, uint queryArraySize);
Datastore* DB_importConfiguration (PGconn* conn);

#endif