#include "Actuator.h"

Actuator* createActuator (Node* node, uint16_t id, uint8_t type, Position* pos) {
    if (!node) {
        return NULL;
    }

    Pixel* pixel = createPixel(node->parentRoom->parentDatastore, NULL, pos);
    if (!pixel) {
        return NULL;
    }

    Room* room = (Room*)node->parentRoom;
    Datastore* datastore = (Datastore*)room->parentDatastore;
    
    if (findActuatorByID(datastore, id)) {
        // There is already a Actuator with this ID
        deletePixel(pixel);
        return NULL;
    }

    Actuator* actuator = (Actuator*)malloc(sizeof(Actuator));
    if (actuator == NULL) {
        // Memory allocation failed
        deletePixel(pixel);
        return NULL;
    }
       
    list_element* elem = listInsert(node->actuators, actuator, NULL);
    if (elem == NULL) {
        // Insertion failed
        free(actuator);
        deletePixel(pixel);
        return NULL;
    }

    actuator->id = id;
    actuator->type = type;
    actuator->parentNode = node;
    actuator->listPtr = elem;
    actuator->pixel = pixel;

    return actuator;
}

bool deleteActuator (Actuator* actuator) {
    if (actuator == NULL) {
        return 1;
    }

    // Remove Actuator from existing rules
    Datastore* datastore = actuator->parentNode->parentRoom->parentDatastore;
    LL_iterator(datastore->rules, rule_elem) {
        Rule* rule = rule_elem->ptr;
        LL_iterator(rule->actuators, ruleActuator_elem) {
            if (actuator == (ruleActuator_elem->ptr)) {
                list_element* rule_remover = listRemove(rule->actuators, ruleActuator_elem);
                if (rule_remover == NULL && listSize(rule->actuators)) {
                    return 1;
                }
                break;
            }
        }
    }

    list_element* elem = actuator->listPtr;
    Node* node = actuator->parentNode;

    deletePixel(actuator->pixel);

    free(actuator);
    list_element* res = listRemove(node->actuators, elem);
    if (res == NULL && listSize(node->actuators)) {
        return 1;
    }

    return 0;
}

Pixel* getActuatorPixel (Actuator* actuator) {
    if (!actuator) {
        return NULL;
    }

    return actuator->pixel;
}

Actuator* findActuatorByID (Datastore* datastore, uint16_t actuatorID) {
    if (!datastore) {
        return NULL;
    }

    LL_iterator(datastore->rooms, room_elem) {
        Room* room = (Room*)room_elem->ptr;
        LL_iterator(room->nodes, node_elem) {
            Node* node = (Node*)node_elem->ptr;
            LL_iterator(node->actuators, actuator_elem) {
                Actuator* actuator = (Actuator*)actuator_elem->ptr;
                if (actuator->id == actuatorID) {
                    return actuator;
                }
            }
        }
    }


    return NULL;
}

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_actuator = {
    NULL,
    "create_table_actuator",
    "CREATE TABLE IF NOT EXISTS sinf.actuator("
    "actuator_id INTEGER NOT NULL PRIMARY KEY,"
    "type INTEGER NOT NULL,"
    "pixel_id INTEGER NOT NULL,"
    "FOREIGN KEY (pixel_id) REFERENCES sinf.pixel(pixel_id) ON UPDATE CASCADE ON DELETE CASCADE"
    ");",
    0
};

DBQuery create_actuator = {
    NULL,
    "create_actuator",
    "INSERT INTO sinf.actuator(actuator_id, type, pixel_id) "
    "VALUES($1, $2, $3);",
    3
};

DBQuery delete_actuator = {
    NULL,
    "delete_actuator",
    "DELETE FROM sinf.actuator WHERE actuator_id=$1;",
    1
};

DBQuery create_table_node_actuator = {
    NULL,
    "create_table_node_actuator",
    "CREATE TABLE IF NOT EXISTS sinf.node_actuator("
    "node_actuator_id SERIAL NOT NULL PRIMARY KEY,"
    "node_id INTEGER NOT NULL,"
    "actuator_id INTEGER NOT NULL,"
    "start_date TIMESTAMP NOT NULL DEFAULT NOW(),"
    "end_date TIMESTAMP,"
    "FOREIGN KEY (node_id) REFERENCES sinf.node(node_id) ON UPDATE CASCADE ON DELETE CASCADE,"
    "FOREIGN KEY (actuator_id) REFERENCES sinf.actuator(actuator_id) ON UPDATE CASCADE ON DELETE CASCADE);",
    0
};

DBQuery add_actuator_to_node = {
    NULL,
    "add_actuator_to_node",
    "INSERT INTO sinf.node_actuator(node_id, actuator_id) "
    "VALUES($1, $2);",
    2
};

DBQuery remove_actuator_from_node = {
    NULL,
    "remove_actuator_from_node",
    "UPDATE sinf.node_actuator "
    "SET end_date = NOW() "
    "WHERE node_id = $1 AND actuator_id = $2 AND end_date IS NULL;",
    2
};

DBQuery create_table_actuator_state = {
    NULL,
    "create_table_actuator_state",
    "CREATE TABLE IF NOT EXISTS sinf.actuator_state("
    "actuator_state_id SERIAL NOT NULL PRIMARY KEY,"
    "actuator_id INTEGER NOT NULL,"
    "value INTEGER NOT NULL,"
    "timestamp TIMESTAMP NOT NULL DEFAULT NOW(),"
    "FOREIGN KEY (actuator_id) REFERENCES sinf.actuator(actuator_id) ON UPDATE CASCADE ON DELETE CASCADE"
    ");",
    0
};

DBQuery create_actuator_state = {
    NULL,
    "create_actuator_state",
    "INSERT INTO sinf.actuator_state(actuator_id, value) "
    "VALUES($1, $2);",
    2
};

void preparePriorityActuatorQueries (list* queryList) {
    addQuerytoList(&create_table_actuator, queryList);
    addQuerytoList(&create_table_node_actuator, queryList);
    addQuerytoList(&create_table_actuator_state, queryList);
}

void prepareActuatorQueries (list* queryList) {
    addQuerytoList(&create_actuator, queryList);
    addQuerytoList(&delete_actuator, queryList);
    addQuerytoList(&add_actuator_to_node, queryList);
    addQuerytoList(&remove_actuator_from_node, queryList);
    addQuerytoList(&create_actuator_state, queryList);
}
