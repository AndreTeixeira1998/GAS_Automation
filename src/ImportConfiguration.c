#include "ImportConfiguration.h"
#include "stdio.h"

char* getMinifiedJSONStringFromFile (const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    int readChar = 0;
    unsigned int jsonSize = 0,
        jsonReserved = 100;
    bool inString = false;
    char* json = malloc(jsonReserved);
    if (!json) {
        fclose(file);
        return NULL;
    }
    json[0] = '\0';

    while ( (readChar = fgetc(file)) != EOF) {
        // Ignore line endings and spaces (if not in string)
        if ( (readChar == ' ' && !inString)
            || readChar == '\n'
            || readChar == '\r') {
            continue;
        }

        // Check if string is being read
        if (readChar == '"' || readChar == '\'') {
            inString = !inString;
        }

        // Check if there is still space in the allocated string
        if (jsonSize == jsonReserved-1) {
            jsonReserved += 100;
            json = realloc(json, jsonReserved);
            if (!json) {
                fclose(file);
                return NULL;
            }
        }

        // Concat current JSON string and new char
        char aux[2] = {readChar, '\0'};
        strcat(json, aux);
        jsonSize++;
    }

    // Trim string to actual size of JSON's minified string
    json = realloc(json, jsonSize+1);

    // Close the file
    fclose(file);

    return json;
}

bool parseSensor (Node* node, cJSON* json_sensor) {
    if (!node || !json_sensor) {
        return 1;
    }

    // Read the type of the sensor from the parsed json
    uint16_t id = 0,
        type = 0,
        posX = 0,
        posY = 0,
        rangeMin = 0,
        rangeMax = 0;
    cJSON* json_id = cJSON_GetObjectItem(json_sensor, "id");
    cJSON* json_type = cJSON_GetObjectItem(json_sensor, "type");
    cJSON* json_posX = cJSON_GetObjectItem(json_sensor, "posX");
    cJSON* json_posY = cJSON_GetObjectItem(json_sensor, "posY");
    cJSON* json_rangeMin = cJSON_GetObjectItem(json_sensor, "rangeMin");
    cJSON* json_rangeMax = cJSON_GetObjectItem(json_sensor, "rangeMax");
    if (cJSON_IsNumber(json_id) &&
        cJSON_IsNumber(json_type) &&
        cJSON_IsNumber(json_posX) &&
        cJSON_IsNumber(json_posY) &&
        cJSON_IsNumber(json_rangeMin) &&
        cJSON_IsNumber(json_rangeMax)) {
        id = (uint16_t)json_id->valueint;
        type = (uint16_t)json_type->valueint;
        posX = (uint16_t)json_posX->valueint;
        posY = (uint16_t)json_posY->valueint;
        rangeMin = (uint16_t)json_rangeMin->valueint;
        rangeMax = (uint16_t)json_rangeMax->valueint;
    }
    else {
        return 1;
    }

    Position position;
    position.x = posX;
    position.y = posY;

    Sensor* sensor = createSensor(node, id, type, &position, rangeMin, rangeMax);
    if (!sensor) {
        return 1;
    }

    return 0;
}

bool parseActuator (Node* node, cJSON* json_actuator) {
    if (!node || !json_actuator) {
        return 1;
    }

    // Read the atributes of the actuator from the parsed json
    uint8_t type = 0;
    uint16_t id = 0,
        posX = 0,
        posY = 0;
    cJSON* json_id = cJSON_GetObjectItem(json_actuator, "id");
    cJSON* json_type = cJSON_GetObjectItem(json_actuator, "type");
    cJSON* json_posX = cJSON_GetObjectItem(json_actuator, "posX");
    cJSON* json_posY = cJSON_GetObjectItem(json_actuator, "posY");
    if (cJSON_IsNumber(json_id) &&
        cJSON_IsNumber(json_type) &&
        cJSON_IsNumber(json_posX) &&
        cJSON_IsNumber(json_posY)) {
        
        id = (uint16_t)json_id->valueint;
        type = (uint8_t)json_type->valueint;
        posX = (uint16_t)json_posX->valueint;
        posY = (uint16_t)json_posY->valueint;
    }
    else {
        return 1;
    }

    Position position;
    position.x = posX;
    position.y = posY;

    // Create actuator instance
    Actuator* actuator = createActuator(node, id, type, &position);
    if (!actuator) {
        return 1;
    }

    return 0;
}

bool parseNode (Room* room, cJSON* json_node) {
    if (!room || !json_node) {
        return 1;
    }

    // Read the id of the node from the parsed json
    uint16_t id = 0;
    cJSON* json_id = cJSON_GetObjectItem(json_node, "id");
    if (cJSON_IsNumber(json_id)) {
        id = (uint16_t)json_id->valueint;
    }
    else {
        return 1;
    }

    // Try to create a node in the room
    Node* node = createNode(room, id);
    if (!node) {
        return 1;
    }

    // Parse the node's sensors
    cJSON *sensors = cJSON_GetObjectItem(json_node, "sensors"),
        *sensor = NULL;
    if (!cJSON_IsArray(sensors)) {
        return 1;
    }
    cJSON_ArrayForEach(sensor, sensors) {
        if(parseSensor(node, sensor)) {
            // Error parsing sensor
            return 1;
        }
    }

    // Parse the node's actuators
    cJSON *actuators = cJSON_GetObjectItem(json_node, "actuators"),
        *actuator = NULL;
    if (!cJSON_IsArray(actuators)) {
        return 1;
    }
    cJSON_ArrayForEach(actuator, actuators) {
        if(parseActuator(node, actuator)) {
            // Error parsing actuator
            return 1;
        }
    }

    return 0;
}

bool parseRule (Datastore* datastore, Rule* parentRule, cJSON* json_rule) {
    if (!json_rule ||
        (!datastore && !parentRule)) {
        return true;
    }


    uint16_t id = 0,
        type = 0,
        value = 0;

    cJSON* json_id = cJSON_GetObjectItem(json_rule, "id");
    cJSON* json_type = cJSON_GetObjectItem(json_rule, "type");
    cJSON* json_value = cJSON_GetObjectItem(json_rule, "value");
    if (cJSON_IsNumber(json_id) &&
        cJSON_IsNumber(json_type) &&
        cJSON_IsNumber(json_value)) {
        
        id = (uint16_t)json_id->valueint;
        type = (uint16_t)json_type->valueint;
        value = (uint16_t)json_value->valueint;
    }
    else {
        return true;
    }

    // Create Rule instance
    Rule* rule = NULL;
    rule = createRule(datastore, parentRule, id, type, value);

    // Check for successfull rule creation
    if (!rule) {
        return true;
    }

    cJSON* json_sensor_array = cJSON_GetObjectItem(json_rule, "sensors"),
        *json_sensor_entry = NULL;;
    if (!cJSON_IsArray(json_sensor_array)) {
        return true;
    }
    cJSON_ArrayForEach(json_sensor_entry, json_sensor_array) {
        if (cJSON_IsNumber(json_sensor_entry)) {
            uint16_t id = (uint16_t)json_sensor_entry->valueint;
            
            Sensor* sensor = findSensorByID(datastore, id);
            if (!sensor) {
                return true;
            }

            if (addSensorToRule(rule, sensor)) {
                return true;
            }
        }
    }

    cJSON* json_actuator_array = cJSON_GetObjectItem(json_rule, "actuators"),
        *json_actuator_entry = NULL;
    if (!cJSON_IsArray(json_actuator_array)) {
        return true;
    }
    cJSON_ArrayForEach(json_actuator_entry, json_actuator_array) {
        if (cJSON_IsNumber(json_actuator_entry)) {
            uint16_t id = (uint16_t)json_actuator_entry->valueint;

            Actuator* actuator = findActuatorByID(datastore, id);
            if (!actuator) {
                return true;
            }

            if (addActuatorToRule(rule, actuator)) {
                return true;
            }
        }
    }

    cJSON* json_childs_array = cJSON_GetObjectItem(json_rule, "childs"),
        *json_childs_entry = NULL;;
    if (!cJSON_IsArray(json_childs_array)) {
        return true;
    }
    cJSON_ArrayForEach(json_childs_entry, json_childs_array) {
        if(!cJSON_IsObject(json_childs_entry)) {
            return true;
        }
        else {
            parseRule(datastore, rule, json_childs_entry);
        }
    }

    return false;
}

bool parseRoom (Datastore* datastore, cJSON* json_room) {
    if (!datastore || !json_room) {
        return 1;
    }

    // Read the id of the room from the parsed json
    uint16_t id = 0;
    cJSON* json_id = cJSON_GetObjectItem(json_room, "id");
    if (cJSON_IsNumber(json_id)) {
        id = (uint16_t)json_id->valueint;
    }
    else {
        return 1;
    }

    // Try to create a room in the datastore
    Room* room = createRoom(datastore, id);
    if (!room) {
        return 1;
    }

    // Read the name of the room from the parsed json
    char* name;
    cJSON* json_name = cJSON_GetObjectItem(json_room, "name");
    if (cJSON_IsString(json_name) && (json_name->valuestring != NULL)) {
        name = json_name->valuestring;
    }
    else {
        return 1;
    }

    // Try to set the room name
    if (setRoomName(room, name)) {
        return 1;
    }

    // Parse the room's nodes
    cJSON *nodes = cJSON_GetObjectItem(json_room, "nodes"),
        *node = NULL;
    if (!cJSON_IsArray(nodes)) {
        return 1;
    }
    cJSON_ArrayForEach(node, nodes) {
        if(parseNode(room, node)) {
            // Error parsing node
            return 1;
        }
    }

    return 0;
}

bool parseExtraPixel (Datastore* datastore, cJSON* json_pixel) {
    if (!datastore || !json_pixel) {
        return true;
    }

    // Read the pixel attributes
    uint16_t posX = 0,
        posY = 0,
        r = 0,
        g = 0,
        b = 0;

    cJSON* json_posX = cJSON_GetObjectItem(json_pixel, "posX");
    cJSON* json_posY = cJSON_GetObjectItem(json_pixel, "posY");
    cJSON* json_r = cJSON_GetObjectItem(json_pixel, "r");
    cJSON* json_g = cJSON_GetObjectItem(json_pixel, "g");
    cJSON* json_b = cJSON_GetObjectItem(json_pixel, "b");
    if (cJSON_IsNumber(json_posX) &&
        cJSON_IsNumber(json_posY) &&
        cJSON_IsNumber(json_r) &&
        cJSON_IsNumber(json_g) &&
        cJSON_IsNumber(json_b)) {

        posX = (uint16_t)json_posX->valueint;
        posY = (uint16_t)json_posY->valueint;
        r = (uint16_t)json_r->valueint;
        g = (uint16_t)json_g->valueint;
        b = (uint16_t)json_b->valueint;
    }
    else {
        return true;
    }

    Position position;
    position.x = posX;
    position.y = posY;

    Color color;
    color.r = r;
    color.g = g;
    color.b = b;

    Pixel* pixel = createPixel(datastore, &color, &position);
    if (!pixel) {
        return true;
    }
    
    return false;
}

Datastore* importConfiguration(const char* filename) {
    
    char* jsonString = getMinifiedJSONStringFromFile(filename);
    if (!jsonString) {
        return NULL;
    }
    
    cJSON* json = cJSON_Parse(jsonString);
    if (!json) {
        free(jsonString);
        return NULL;
    }

    Datastore* datastore = createDatastore();
    if (!datastore) {
        free(jsonString);
        cJSON_Delete(json);
        return NULL;
    }

    // Parse the room's data from the configuration file
    cJSON *rooms = cJSON_GetObjectItem(json, "rooms"),
        *room = NULL;
    if (!cJSON_IsArray(rooms)) {
        deleteDatastore(datastore);
        cJSON_Delete(json);
        free(jsonString);
        return NULL;
    }
    cJSON_ArrayForEach(room, rooms) {
        if(parseRoom(datastore, room)) {
            // Error parsing room
            deleteDatastore(datastore);
            cJSON_Delete(json);
            free(jsonString);
            return NULL;
        }
    }

    // Parse the rule's data from the configuration file
    cJSON *rules = cJSON_GetObjectItem(json, "rules"),
        *rule = NULL;
    if (!cJSON_IsArray(rules)) {
        deleteDatastore(datastore);
        cJSON_Delete(json);
        free(jsonString);
        return NULL;
    }
    cJSON_ArrayForEach(rule, rules) {
        if(parseRule(datastore, NULL, rule)) {
            // Error parsing rule
            deleteDatastore(datastore);
            cJSON_Delete(json);
            free(jsonString);
            return NULL;
        }
    }

    // Parse the extra pixel's data from the configuration file
    cJSON *pixels = cJSON_GetObjectItem(json, "pixels"),
        *pixel = NULL;
    if (!cJSON_IsArray(pixels)) {
        deleteDatastore(datastore);
        cJSON_Delete(json);
        free(jsonString);
        return NULL;
    }
    cJSON_ArrayForEach(pixel, pixels) {
        if(parseExtraPixel(datastore, pixel)) {
            // Error parsing extra Pixel
            deleteDatastore(datastore);
            cJSON_Delete(json);
            free(jsonString);
            return NULL;
        }
    }

    // Free resources
    //printf("%s\n", jsonString);
    cJSON_Delete(json);
    free(jsonString);
    
    return datastore;
}