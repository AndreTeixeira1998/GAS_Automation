#include "Rule.h"

Rule* createRule (Datastore* datastore, Rule* parentRule, uint16_t id, uint16_t type, uint16_t value) {
    if (!datastore) {
        return NULL;
    }

    if (findRuleByID(datastore, id)) {
        // There's already a rule with the specified ID
        return NULL;
    }

    // Alocate memory for this rule
    Rule* rule = (Rule*)malloc(sizeof(Rule));
    if (!rule) {
        return NULL;
    }

    list* sensors = newList();
    if (sensors == NULL) {
        free(rule);
        return NULL;
    }

    list* actuators = newList();
    if (actuators == NULL) {
        deleteList(sensors);
        free(rule);
        return NULL;
    }

    list* childs = newList();
    if (childs == NULL) {
        deleteList(sensors);
        deleteList(actuators);
        free(rule);
        return NULL;
    }

    list* profiles = newList();
    if (profiles == NULL) {
        deleteList(childs);
        deleteList(sensors);
        deleteList(actuators);
        free(rule);
        return NULL;
    }

    rule->parentDatastore = datastore;
    rule->parentRule = parentRule;
    rule->id = id;
    rule->sensors = sensors;
    rule->actuators = actuators;
    rule->operation = type;
    rule->value = value;
    rule->childs = childs;
    rule->profiles = profiles;

    list_element *elem = NULL,
        *parentRuleElem = NULL;

    if (parentRule) { // Insert rule in parent rule
        parentRuleElem = listInsert(parentRule->childs, rule, NULL);
    
        if (parentRuleElem == NULL) {
            // Insertion failed
            deleteList(sensors);
            deleteList(actuators);
            deleteList(childs);
            deleteList(profiles);
            free(rule);
            return NULL;
        }
    }

    // Insert rule in datastore
    elem = listInsert(datastore->rules, rule, NULL);
    if (elem == NULL) {
        // Insertion failed
        if (parentRule) {
            listRemove(parentRule->childs, parentRuleElem);
        }
        deleteList(sensors);
        deleteList(actuators);
        deleteList(childs);
        deleteList(profiles);
        free(rule);
        return NULL;
    }

    rule->listPtr = elem;
    rule->listPtr_parentRule = parentRuleElem;

    return rule;
}

bool deleteRule (Rule* rule) {
    if (!rule) {
        return true;
    }

    uint16_t retVal = 0;

    deleteList(rule->sensors);
    deleteList(rule->actuators);
    deleteList(rule->profiles);

    // Delete all childs
    list_element* aux = listStart(rule->childs);
    while (aux != NULL) {
        if (deleteRule(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(rule->childs);
    }
    deleteList(rule->childs);


    list_element *elem = rule->listPtr,
        *res;
    
    res = listRemove(rule->parentDatastore->rules, elem);
    if (res == NULL && listSize(rule->parentDatastore->rules)) {
        // TODO check this for logic error
        retVal++;
    }

    if (rule->parentRule) {
        elem = rule->listPtr_parentRule;
        res = listRemove(rule->parentRule->childs, elem);
        if (res == NULL && listSize(rule->parentRule->childs)) {
            retVal++;
        }
    }
    
    free(rule);

    return retVal > 0 ? true : false;
}

bool addSensorToRule (Rule* rule, Sensor* sensor) {
    if (!rule || !sensor) {
        return true;
    }

    list_element* elem = listInsert(rule->sensors, sensor, NULL);
    if (elem == NULL) {
        // Insertion failed
        return true;
    }

    return false;
}

bool addActuatorToRule (Rule* rule, Actuator* actuator) {
    if (!rule || !actuator) {
        return true;
    }

    list_element* elem = listInsert(rule->actuators, actuator, NULL);
    if (elem == NULL) {
        // Insertion failed
        return true;
    }

    return false;
}

bool evaluateRule (Rule* rule) {
    if (!rule) {
        return false;
    }

    // Test all childs
    LL_iterator(rule->childs, child_elem) {
        Rule* child = child_elem->ptr;
        if (evaluateRule(child)) {
            // One Child is verified
            break;
        }
        return false;
    }

    // Test sensor values against rule value given the rule operation
    LL_iterator(rule->sensors, sensor_elem) {
        Sensor* sensor = sensor_elem->ptr;
        float val = 0;
        
        switch(rule->operation) {
            case TYPE_RULE_LESS_THEN:
                if ( !(getSensorValue(sensor) < rule->value) ) {
                    return false;
                }
                break;

            case TYPE_RULE_GREATER_THEN:
                if ( !(getSensorValue(sensor) > rule->value) ) {
                    return false;
                }
                break;

            case TYPE_RULE_EQUAL_TO:
                if ( !(getSensorValue(sensor) == rule->value) ) {
                    return false;
                }
                break;

            case TYPE_RULE_WITHIN_MARGIN:
                val = getSensorValue(sensor);
                if ( !((val > ((float)(rule->value))*(0.95)) &&
                    (val < ((float)(rule->value))*(1.05)) )) {
                    return false;
                }
                break;
                
            default:
                return false;
                break;
        }
    }


    return true;
}

bool executeRules (Datastore* datastore) {
    if (!datastore) {
        return true;
    }

    Color colorActive,
        colorInactive;
    
    // Green
    colorActive.r = 0;
    colorActive.g = 255;
    colorActive.b = 0;

    // Red
    colorInactive.r = 255;
    colorInactive.g = 0;
    colorInactive.b = 0;

    LL_iterator(datastore->rules, rule_elem) {
        Rule* rule = rule_elem->ptr;
        bool active = evaluateRule(rule);

        // Rule is active
        LL_iterator(rule->actuators, actuator_elem) {
            Actuator* actuator = actuator_elem->ptr;
            Pixel* pixel = getActuatorPixel(actuator);
            if (setPixelColor(pixel, active ? &colorActive : &colorInactive)) {
                return true;
            }
        }
    }

    return false;
}

Rule* findRuleByIDinLinkedList (list* linkedList, uint16_t id) {
    if (!linkedList) {
        return NULL;
    }

    LL_iterator(linkedList, rule_elem) {
        Rule* rule = (Rule*)rule_elem->ptr;

        if (rule->id == id) {
            return rule;
        }

        Rule* result = findRuleByIDinLinkedList(rule->childs, id);
        if (result) {
            return result;
        }
    }

    return NULL;
}

Rule* findRuleByID (Datastore* datastore, uint16_t id) {
    if (!datastore) {
        return NULL;
    }

    return findRuleByIDinLinkedList(datastore->rules, id);
}

bool addProfileToRule (Rule* rule, Profile* profile) {
    if (!rule || !profile) {
        return true;
    }

    list_element* elem = listInsert(rule->profiles, profile, NULL);
    if (!elem) {
        // Error while inserting
        return true;
    }

    return false;
}

bool removeProfileFromRule (Rule* rule, Profile* profile) {
    if (!rule || !profile) {
        return true;
    }

    LL_iterator(rule->profiles, rule_profile_elem) {
        Profile* rule_profile = (Profile*)rule_profile_elem->ptr;
        if (rule_profile == profile) {
            list_element* res = listRemove(rule->profiles, rule_profile_elem);
            if (res == NULL && listSize(profile->parentDatastore->profiles)) {
                // TODO check this for logic error
                return true;
            }
        }
    }

    return false;
}

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_rule = {
    NULL,
    "create_table_rule",
    "CREATE TABLE IF NOT EXISTS sinf.rule("
    "rule_id SERIAL NOT NULL PRIMARY KEY,"
    "operation INTEGER NOT NULL,"
    "value INTEGER NOT NULL,"
    "parent_id INTEGER,"
    "FOREIGN KEY (parent_id) REFERENCES sinf.rule(rule_id) ON UPDATE CASCADE ON DELETE CASCADE);",
    0
};

DBQuery create_rule = {
    NULL,
    "create_rule",
    "INSERT INTO sinf.rule(operation, value) "
    "VALUES($1, $2);",
    2
};

DBQuery create_rule_with_parent = {
    NULL,
    "create_rule_with_parent",
    "INSERT INTO sinf.rule(operation, value, parent_id) "
    "VALUES($1, $2, $3);",
    3
};

DBQuery delete_rule = {
    NULL,
    "delete_rule",
    "DELETE FROM sinf.rule WHERE rule_id=$1;",
    1
};

void preparePriorityRuleQueries (list* queryList) {
    addQuerytoList(&create_table_rule, queryList);
}

void prepareRuleQueries (list* queryList) {
    addQuerytoList(&create_rule, queryList);
    addQuerytoList(&create_rule_with_parent, queryList);
    addQuerytoList(&delete_rule, queryList);
}
