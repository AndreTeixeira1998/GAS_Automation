#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>

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
void DB_prepareAllQueries (PGconn* conn, list* queryList);
void DB_exec (list* queryList, char* query_name, char* paramValues[]);
Datastore* DB_importConfiguration (PGconn* conn);

void addQuerytoList (DBQuery* query, list* queryList);

#endif