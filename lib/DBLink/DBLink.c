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

    // Priority queries first
    LL_iterator(queryList, query_elem) {
        DBQuery* query = (DBQuery*)query_elem->ptr;
        if (!query->conn) {
            query->conn = conn;
            __DB_prepareQuery(query);
        }
    }
}

void DB_preparePriorityQueries (PGconn* conn, list* queryList) {
    preparePriorityProfileQueries(queryList);
    preparePriorityPixelQueries(queryList);
    preparePriorityRuleQueries(queryList);
    preparePriorityRoomQueries(queryList);
    preparePriorityNodeQueries(queryList);
    preparePrioritySensorQueries(queryList);
    preparePriorityActuatorQueries(queryList);
    DB_prepareSQLQueries(conn, queryList);
}

void DB_prepareRegularQueries (PGconn* conn, list* queryList) {
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

PGresult* __DB_exec (DBQuery* query, char* paramValues[]) {
    if (!query) {
        return NULL;
    }

    int paramLengths[query->nParams];
    int paramFormats[query->nParams];
    for (int i = 0; i < query->nParams; i++) {
        paramLengths[i] = strlen(paramValues[i]);
        paramFormats[i] = 0;
    }

    if (!query->conn) {
        return NULL;
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

    return stmt;
}

PGresult* DB_exec (list* queryList, char* query_name, char* paramValues[]) {
    if (!queryList) {
        return NULL;
    }

    DBQuery* query = findQueryByName(queryList, query_name);

    return __DB_exec(query, paramValues);
}

void DB_uploadConfiguration (Datastore* datastore, list* queryList) {
    if (!datastore) {
        return;
    }

    // PIXELS
    LL_iterator(datastore->pixels, pixel_elem) {
        Pixel* pixel = (Pixel*)pixel_elem->ptr;

        DBQuery* query = findQueryByName(queryList, "create_pixel");
        if (!query) {
            fprintf(stderr, "Error uploading configuration to DB.\n");
            return;
        }

        char* params[query->nParams];
        for (int i = 0; i < query->nParams; i++) {
            params[i] = malloc(12*sizeof(char));
        }
        sprintf(params[0], "%d", pixel->pos->x);
        sprintf(params[1], "%d", pixel->pos->y);

        PGresult* stmt = __DB_exec(
            query,
            params
        );

        int tuples = PQntuples(stmt);
        if (tuples != 1) {
            fprintf(stderr, "Error uploading configuration to DB.\n");
            return;
        }
        char* remote_id_str = PQgetvalue(stmt, 0, 0);
        uint16_t remote_id = strtol(remote_id_str, (char **)NULL, 10);
        pixel->remote_id = remote_id;

        for (int i = 0; i < query->nParams; i++) {
            free(params[i]);
        }
    }

    // PROFILES
    LL_iterator(datastore->profiles, profile_elem) {
        Profile* profile = (Profile*)profile_elem->ptr;

        DBQuery* query = NULL;
        if (profile->name) {
            query = findQueryByName(queryList, "create_named_profile");
        }
        else {
            query = findQueryByName(queryList, "create_profile");
        }

        if (!query) {
            fprintf(stderr, "Error uploading configuration to DB.\n");
            return;
        }

        char* params[query->nParams];
        params[0] = malloc(12*sizeof(char));
        params[1] = malloc(12*sizeof(char));
        params[2] = malloc(12*sizeof(char));
        sprintf(params[0], "%d", profile->id);
        strftime(params[1], 12, "%H:%M", &(profile->start));
        strftime(params[2], 12, "%H:%M", &(profile->end));
        if (profile->name) {
            params[3] = profile->name;
        }


        __DB_exec(
            query,
            params
        );

        free(params[0]);
        free(params[1]);
        free(params[2]);
    }

    // ROOMS
    LL_iterator(datastore->rooms, room_elem) {
        Room* room = (Room*)room_elem->ptr;

        DBQuery* query = findQueryByName(queryList, "create_room");
        if (!query) {
            fprintf(stderr, "Error uploading configuration to DB.\n");
            return;
        }

        char* params[query->nParams];
        params[0] = malloc(12*sizeof(char));
        sprintf(params[0], "%d", room->id);
        params[1] = room->name;

        __DB_exec(
            query,
            params
        );

        // DO NOT FREE THE ROOM NAME AS IT IS NOT A COPY
        free(params[0]); // free id
    
        
        // NODES, SENSORS & ACTUATORS
        LL_iterator(room->nodes, node_elem) {
            Node* node = (Node*)node_elem->ptr;

            // add node
            DBQuery* query = findQueryByName(queryList, "create_node");
            if (!query) {
                fprintf(stderr, "Error uploading configuration to DB.\n");
                return;
            }

            char* params[query->nParams];
            for (int i = 0; i < query->nParams; i++) {
                params[i] = malloc(12*sizeof(char));
            }
            
            sprintf(params[0], "%d", node->id);

            __DB_exec(
                query,
                params
            );

            for (int i = 0; i < query->nParams; i++) {
                free(params[i]);
            }

            // Add node to room
            query = findQueryByName(queryList, "add_node_to_room");
            if (!query) {
                fprintf(stderr, "Error uploading configuration to DB.\n");
                return;
            }

            char* params1[query->nParams];
            for (int i = 0; i < query->nParams; i++) {
                params1[i] = malloc(12*sizeof(char));
            }
            
            sprintf(params1[0], "%d", room->id);
            sprintf(params1[1], "%d", node->id);

            __DB_exec(
                query,
                params1
            );

            for (int i = 0; i < query->nParams; i++) {
                free(params1[i]);
            }


            // add sensor
            LL_iterator(node->sensors, sensor_elem) {
                Sensor* sensor = (Sensor*)sensor_elem->ptr;
                query = findQueryByName(queryList, "create_sensor");
                if (!query) {
                    fprintf(stderr, "Error uploading configuration to DB.\n");
                    return;
                }

                char* params[query->nParams];
                for (int i = 0; i < query->nParams; i++) {
                    params[i] = malloc(12*sizeof(char));
                }
                
                sprintf(params[0], "%d", sensor->id);
                sprintf(params[1], "%d", sensor->type);
                sprintf(params[2], "%d", sensor->pixel->remote_id);

                __DB_exec(
                    query,
                    params
                );

                for (int i = 0; i < query->nParams; i++) {
                    free(params[i]);
                }

                query = findQueryByName(queryList, "add_sensor_to_node");
                if (!query) {
                    fprintf(stderr, "Error uploading configuration to DB.\n");
                    return;
                }

                char* params1[query->nParams];
                for (int i = 0; i < query->nParams; i++) {
                    params1[i] = malloc(12*sizeof(char));
                }
                
                sprintf(params1[0], "%d", node->id);
                sprintf(params1[1], "%d", sensor->id);

                __DB_exec(
                    query,
                    params1
                );

                for (int i = 0; i < query->nParams; i++) {
                    free(params1[i]);
                }
            }

            // add actuator
            LL_iterator(node->actuators, actuator_elem) {
                Actuator* actuator = (Actuator*)actuator_elem->ptr;
                query = findQueryByName(queryList, "create_actuator");
                if (!query) {
                    fprintf(stderr, "Error uploading configuration to DB.\n");
                    return;
                }

                char* params[query->nParams];
                for (int i = 0; i < query->nParams; i++) {
                    params[i] = malloc(12*sizeof(char));
                }
                
                sprintf(params[0], "%d", actuator->id);
                sprintf(params[1], "%d", actuator->type);
                sprintf(params[2], "%d", actuator->pixel->remote_id);

                __DB_exec(
                    query,
                    params
                );

                for (int i = 0; i < query->nParams; i++) {
                    free(params[i]);
                }

                query = findQueryByName(queryList, "add_actuator_to_node");
                if (!query) {
                    fprintf(stderr, "Error uploading configuration to DB.\n");
                    return;
                }

                char* params1[query->nParams];
                for (int i = 0; i < query->nParams; i++) {
                    params1[i] = malloc(12*sizeof(char));
                }
                
                sprintf(params1[0], "%d", node->id);
                sprintf(params1[1], "%d", actuator->id);

                __DB_exec(
                    query,
                    params1
                );

                for (int i = 0; i < query->nParams; i++) {
                    free(params1[i]);
                }
            }
        }
    }


    // RULES
    LL_iterator(datastore->rules, rule_elem) {
        Rule* rule = (Rule*)rule_elem->ptr;

        DBQuery* query = NULL;
        if (rule->parentRule) {
            query = findQueryByName(queryList, "create_rule_with_parent");
        }
        else {
            query = findQueryByName(queryList, "create_rule");
        }
        if (!query) {
            fprintf(stderr, "Error uploading configuration to DB.\n");
            return;
        }

        char* params[query->nParams];
        for (int i = 0; i < query->nParams; i++) {
            params[i] = malloc(12*sizeof(char));
        }
        
        sprintf(params[0], "%d", rule->id);
        sprintf(params[1], "%d", rule->operation);
        sprintf(params[2], "%d", rule->value);
        if (rule->parentRule) {
            sprintf(params[3], "%d", rule->parentRule->id);
        }

        __DB_exec(
            query,
            params
        );

        for (int i = 0; i < query->nParams; i++) {
            free(params[i]);
        }

        LL_iterator(rule->sensors, sensor_elem) {
            Sensor* sensor = (Sensor*)sensor_elem->ptr;
            query = findQueryByName(queryList, "add_sensor_to_rule");
            if (!query) {
                fprintf(stderr, "Error uploading configuration to DB.\n");
                return;
            }

            char* params[query->nParams];
            for (int i = 0; i < query->nParams; i++) {
                params[i] = malloc(12*sizeof(char));
            }
            
            sprintf(params[0], "%d", sensor->id);
            sprintf(params[1], "%d", rule->id);

            __DB_exec(
                query,
                params
            );

            for (int i = 0; i < query->nParams; i++) {
                free(params[i]);
            }
        }

        LL_iterator(rule->actuators, actuator_elem) {
            Actuator* actuator = (Actuator*)actuator_elem->ptr;
            query = findQueryByName(queryList, "add_actuator_to_rule");
            if (!query) {
                fprintf(stderr, "Error uploading configuration to DB.\n");
                return;
            }

            char* params[query->nParams];
            for (int i = 0; i < query->nParams; i++) {
                params[i] = malloc(12*sizeof(char));
            }
            
            sprintf(params[0], "%d", actuator->id);
            sprintf(params[1], "%d", rule->id);

            __DB_exec(
                query,
                params
            );

            for (int i = 0; i < query->nParams; i++) {
                free(params[i]);
            }
        }

        LL_iterator(rule->profiles, profile_elem) {
            Profile* profile = (Profile*)profile_elem->ptr;
            query = findQueryByName(queryList, "add_profile_to_rule");
            if (!query) {
                fprintf(stderr, "Error uploading configuration to DB.\n");
                return;
            }

            char* params[query->nParams];
            for (int i = 0; i < query->nParams; i++) {
                params[i] = malloc(12*sizeof(char));
            }
            
            sprintf(params[0], "%d", profile->id);
            sprintf(params[1], "%d", rule->id);

            __DB_exec(
                query,
                params
            );

            for (int i = 0; i < query->nParams; i++) {
                free(params[i]);
            }
        }
    }

    return;
}

Datastore* DB_importConfiguration (PGconn* conn, list* queryList) {
    if (!conn || PQstatus(conn) != CONNECTION_OK || !queryList) {
        return NULL;
    }

    // Create Datastore
    Datastore* datastore = createDatastore();
    if (!datastore) {
        return NULL;
    }

    return datastore;
}
