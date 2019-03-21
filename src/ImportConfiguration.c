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
    uint16_t type = 0,
        posX = 0,
        posY = 0;
    cJSON* json_type = cJSON_GetObjectItem(json_sensor, "type");
    cJSON* json_posX = cJSON_GetObjectItem(json_sensor, "posX");
    cJSON* json_posY = cJSON_GetObjectItem(json_sensor, "posY");
    if (cJSON_IsNumber(json_type) &&
        cJSON_IsNumber(json_posX) &&
        cJSON_IsNumber(json_posY)) {
        type = (uint16_t)json_type->valueint;
        posX = (uint16_t)json_posX->valueint;
        posY = (uint16_t)json_posY->valueint;
    }
    else {
        return 1;
    }

    Position position;
    position.x = posX;
    position.y = posY;

    Sensor* sensor = createSensor(node, type, &position);
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

    //printf("%s\n", jsonString);
    free(jsonString);
    return datastore;
}