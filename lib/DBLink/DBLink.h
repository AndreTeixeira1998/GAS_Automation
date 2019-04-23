#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>
#include "DataStructure.h"

char* getConnectionStringFromFile (const char* filename);
Datastore* importConfigurationFromDB (PGconn* conn);

#endif