#include "DBLink.h"

char* getConnectionStringFromFile (const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    int readChar = 0;
    unsigned int strSize = 0,
        strReserved = 100;
    char* str = malloc(strReserved);
    bool EOL = false;
    if (!str) {
        fclose(file);
        return NULL;
    }
    str[0] = '\0';

    while ( (readChar = fgetc(file)) != EOF) {
        // Ignore line endings
        if ( readChar == '\n'
            || readChar == '\r') {
            
            if (!EOL) {
                readChar = ' ';
                EOL = true;
            }
            else {
                continue;
            }
        }
        else {
            // Line-endings ended :)
            EOL = false;
        }

        // Check if there is still space in the allocated string
        if (strSize == strReserved-1) {
            strReserved += 100;
            str = realloc(str, strReserved);
            if (!str) {
                fclose(file);
                return NULL;
            }
        }

        // Concat current string and new char
        char aux[2] = {readChar, '\0'};
        strcat(str, aux);
        strSize++;
    }

    // Trim string to actual size required
    str = realloc(str, strSize+1);

    // Close the file
    fclose(file);

    return str;
}

typedef struct {
    PGconn* conn;
    char* name;
    char* query;
    int nParams;
} DBQuery;

void __DB_prepareQuery (DBQuery query) {
    
    PGresult* stmt = PQprepare(
        query.conn,
        query.name,
        query.query,
        query.nParams,
        NULL);
    
    fprintf(stderr, "%s", PQresultErrorMessage(stmt));
}

void DB_prepareAllSQLQueries (PGconn* conn) {
    if (!conn || PQstatus(conn) != CONNECTION_OK) {
        return true;
    }

    int nQueries = 2; // Change to match number of nodes below

    DBQuery nodes[2] = {
        // {
        //     connection_to_db,
        //     query_name,
        //     query_part_1
        //     query_part_2,
        //     0
        // },
        {
            conn,
            "create_node",
            "INSERT INTO sinf.node "
            "DEFAULT VALUES;",
            0
        },
        {
            conn,
            "create_actuator",
            "INSERT INTO sinf.actuator(type, pixel_id) "
            "VALUES($1, $2);",
            2
        }
    };

    for (int i = 0; i < nQueries; i++) {
        __DB_prepareQuery(nodes[i]);
    }
}

Datastore* DB_importConfiguration (PGconn* conn) {
    if (!conn || PQstatus(conn) != CONNECTION_OK) {
        return NULL;
    }

    // Create Datastore
    Datastore* datastore = createDatastore();
    if (!datastore) {
        return NULL;
    }

    return datastore;
}
