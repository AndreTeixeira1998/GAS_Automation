#include "DBLink.h"

Datastore* importConfigurationFromDB (PGconn* conn) {
    if (!conn || PQstatus(conn) != CONNECTION_MADE) {
        return NULL;
    }

    // Create Datastore
    Datastore* datastore = createDatastore();
    if (!datastore) {
        return NULL;
    }

    return NULL;
}
