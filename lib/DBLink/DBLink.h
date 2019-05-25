#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>
#include <stdlib.h>

typedef struct _dbquery DBQuery;

#include "Datastore.h"
#include "Profile.h"
#include "Pixel.h"

struct _dbquery {
    PGconn* conn;
    char* name;
    char* query;
    int nParams;
};

char* getConnectionStringFromFile (const char* filename);
void DB_prepareSQLQueries (PGconn* conn, list* queryList);
void DB_preparePriorityQueries (PGconn* conn, list* queryList);
void DB_prepareRegularQueries (PGconn* conn, list* queryList);
PGresult* __DB_exec (DBQuery* query, char* paramValues[]);
PGresult* DB_exec (list* queryList, char* query_name, char* paramValues[]);
void DB_uploadConfiguration (Datastore* datastore, list* queryList);
Datastore* DB_importConfiguration (PGconn* conn, list* queryList);

void addQuerytoList (DBQuery* query, list* queryList);

#endif