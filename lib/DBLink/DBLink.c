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
        return;
    }

    for (int i = 0; i < queryArraySize; i++) {
        __DB_prepareQuery(queryArray[i]);
    }
}

void DB_createNode () {

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
