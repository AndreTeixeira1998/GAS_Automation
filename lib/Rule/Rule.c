#include "Rule.h"

Rule* createRule (Room* room, Rule* parent, uint16_t type, uint16_t value) {
    if ((!room && !parent) || 
        (room && parent)) {
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

    rule->parentRoom = room;
    rule->parentRule = parent;
    rule->sensors = sensors;
    rule->actuators = actuators;
    rule->operation = type;
    rule->value = value;
    rule->childs = childs;

    list_element* elem = NULL;

    if (room) { // Insert rule in room
        elem = listInsert(room->rules, rule, NULL);
    }
    else if (parent) { // Insert rule in parent rule
        elem = listInsert(parent->childs, rule, NULL);
    }
    
    if (elem == NULL) {
        // Insertion failed
        deleteList(sensors);
        deleteList(actuators);
        deleteList(childs);
        free(rule);
        return NULL;
    }

    rule->listPtr = elem;

    return rule;
}

bool deleteRule (Rule* rule) {
    if (!rule) {
        return true;
    }

    uint16_t retVal = 0;

    deleteList(rule->sensors);
    deleteList(rule->actuators);

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
    
    if (rule->parentRoom) {
        res = listRemove(rule->parentRoom->rules, elem);
        if (res == NULL && listSize(rule->parentRoom->rules)) {
            retVal++;
        }
    }
    else if (rule->parentRule) {
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

    LL_iterator(datastore->rooms, room_elem) {
        Room* room = room_elem->ptr;
        LL_iterator(room->rules, rule_elem) {
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
    }

    return false;
}