#ifndef __DBLINK__
#define __DBLINK__

#include <libpq-fe.h>
#include "DataStructure.h"

Datastore* importConfigurationFromDB (PGconn* conn);

#endif