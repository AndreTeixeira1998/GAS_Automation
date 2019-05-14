#include "DBLink.h"

void addQuerytoList (DBQuery* query, list* queryList) {
    if (!query || !queryList) {
        return;
    }

    listInsert(queryList, query, NULL);
}

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

void __DB_prepareQuery (DBQuery* query) {
    
    PGresult* stmt = PQprepare(
        query->conn,
        query->name,
        query->query,
        query->nParams,
        NULL);
    
    fprintf(stderr, "%s", PQresultErrorMessage(stmt));
}

void DB_prepareSQLQueries (PGconn* conn, list* queryList) {
    if (!conn || PQstatus(conn) != CONNECTION_OK) {
        return;
    }

    LL_iterator(queryList, query_elem) {
        DBQuery* query = (DBQuery*)query_elem->ptr;
        query->conn = conn;
        __DB_prepareQuery(query);
    }
}

void DB_prepareAllQueries (PGconn* conn, list* queryList) {
    prepareProfileQueries(queryList);
    preparePixelQueries(queryList);
    prepareRuleQueries(queryList);
    prepareRoomQueries(queryList);
    prepareNodeQueries(queryList);
    prepareSensorQueries(queryList);
    prepareActuatorQueries(queryList);
    DB_prepareSQLQueries(conn, queryList);
}

DBQuery* findQueryByName (list* queryList, char* query_name) {
    if (!queryList || ! query_name) {
        return NULL;
    }

    DBQuery* query = NULL;
    LL_iterator(queryList, query_elem) {
        DBQuery* query_obj = (DBQuery*)query_elem->ptr;
        if (!strcmp(query_obj->name, query_name)) {
            query = query_obj;
        }
    }
    if (!query) {
        fprintf(stderr, "Query name not found.");
        return NULL;
    }

    return query;
}

void __DB_exec (list* queryList, DBQuery* query, char* paramValues[]) {
    if (!queryList || !query) {
        return;
    }

    int paramLengths[query->nParams];
    int paramFormats[query->nParams];
    for (int i = 0; i < query->nParams; i++) {
        paramLengths[i] = strlen(paramValues[i]);
        paramFormats[i] = 0;
    }

    PGresult* stmt = PQexecPrepared(
        query->conn,
        query->name,
        query->nParams,
        paramValues,
        paramLengths,
        paramFormats,
        0
    );

    fprintf(stderr, "%s", PQresultErrorMessage(stmt));
}

void DB_exec (list* queryList, char* query_name, char* paramValues[]) {
    if (!queryList) {
        return;
    }

    DBQuery* query = findQueryByName(queryList, query_name);

    __DB_exec(queryList, query, paramValues);

    return;
}

void DB_uploadConfiguration (Datastore* datastore, list* queryList) {
    if (!datastore) {
        return;
    }

    LL_iterator(datastore->pixels, pixel_elem) {
        Pixel* pixel = (Pixel*)pixel_elem->ptr;

        DBQuery* query = findQueryByName(queryList, "create_pixel");

        char* params[query->nParams];
        params[0] = malloc(12*sizeof(char));
        params[1] = malloc(12*sizeof(char));
        sprintf(params[0], "%d", pixel->pos->x);
        sprintf(params[1], "%d", pixel->pos->y);

        __DB_exec(queryList,
            query,
            params
        );

        for (int i = 0; i < query->nParams; i++) {
            free(params[i]);
        }
    }

    LL_iterator(datastore->rules, rule_elem) {
        Rule* rule = (Rule*)rule_elem->ptr;

        DBQuery* query = NULL;
        if (rule->parentRule) {
            query = findQueryByName(queryList, "create_rule_with_parent");
        }
        else {
            query = findQueryByName(queryList, "create_rule");
        }

        char* params[query->nParams];
        for (int i = 0; i < query->nParams; i++) {
            params[i] = malloc(12*sizeof(char));
        }
        
        sprintf(params[0], "%d", rule->operation);
        sprintf(params[1], "%d", rule->value);
        if (rule->parentRule) {
            sprintf(params[2], "%d", rule->parentRule->id);
        }

        __DB_exec(queryList,
            query,
            params
        );

        for (int i = 0; i < query->nParams; i++) {
            free(params[i]);
        }
    }

    return;
}

Datastore* DB_importConfiguration (PGconn* conn, list* queryList) {
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
