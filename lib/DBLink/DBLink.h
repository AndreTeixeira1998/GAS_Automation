#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>
#include "DataStructure.h"

char* getConnectionStringFromFile (const char* filename);
void DB_prepareAllSQLQueries (PGconn* conn);
Datastore* DB_importConfiguration (PGconn* conn);

#endif