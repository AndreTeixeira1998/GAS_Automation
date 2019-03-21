#include "DataStructure.h"


bool isValidSensorType (uint8_t type) {
    if (type <= N_TYPE_SENSOR) {
        return true;
    }

    return false;
}

float voltageSensorValue (uint16_t value) {
    const float VRef = 1.5;
    return (float)value/4096 * VRef;
}

float temperatureSensorValue (uint16_t value) {
    return -39.6 + 0.01 * (float)value;
}

float humiditySensorValue (uint16_t value) {
    // Using (value*value) to avoid import math.h
    return -2.0468 + 0.0367 * (float)value -0.0000015955 * ((float)value*(float)value);
}

float lightSensorValue (uint16_t value) {
    return 6250 * ((float)value/4096) * 1.5;
}

float currentSensorValue (uint16_t value) {
    return 769 * ((float)value/4096) * 1.5;
}

sensorValueCalculator* sensorCalculatorFunctionPointer (uint8_t type) {
    if (!isValidSensorType(type)) {
        return NULL;
    }

    sensorValueCalculator* ptr = NULL;

    switch (type)
    {
        case TYPE_SENSOR_VOLTAGE:
            ptr = VALUE_CALCULATOR(TYPE_SENSOR_VOLTAGE);
            break;
        case TYPE_SENSOR_TEMPERATURE:
            ptr = VALUE_CALCULATOR(TYPE_SENSOR_TEMPERATURE);
            break;
        case TYPE_SENSOR_HUMIDITY:
            ptr = VALUE_CALCULATOR(TYPE_SENSOR_HUMIDITY);
            break;
        case TYPE_SENSOR_LIGHT:
            ptr = VALUE_CALCULATOR(TYPE_SENSOR_LIGHT);
            break;
        case TYPE_SENSOR_CURRENT:
            ptr = VALUE_CALCULATOR(TYPE_SENSOR_CURRENT);
            break;
    }
    
    return ptr;
}

Datastore* createDatastore () {
    Datastore* datastore = (Datastore*)malloc(sizeof(Datastore));
    if (datastore == NULL) {
        // Memory allocation failed
        return NULL;
    }

    list* rooms = newList();
    if (rooms == NULL) {
        // Error creating list
        free(datastore);
        return NULL;
    }

    list* pixels = newList();
    if (pixels == NULL) {
        // Error creating list
        deleteList(rooms);
        free(datastore);
        return NULL;
    }

    datastore->rooms = rooms;
    datastore->pixels = pixels;

    return datastore;
}

Room* createRoom (Datastore* datastore, uint16_t id) {
    if (!datastore || findRoomByID(datastore, id)) {
        return NULL;
    }

    Room* room = (Room*)malloc(sizeof(Room));
    if (room == NULL) {
        // Memory allocation failed
        return NULL;
    }

    list_element* elem = listInsert(datastore->rooms, room, NULL);
    if (elem == NULL) {
        // Insertion failed
        free(room);
        return NULL;
    }

    list* nodes = newList();
    if (nodes == NULL) {
        free(room);
        listRemove(datastore->rooms, elem);
        return NULL;
    }

    room->id = id;
    room->parentDatastore = datastore;
    room->listPtr = elem;
    room->name = NULL;
    room->nodes = nodes;

    return room;
}

Node* createNode (Room* room, uint16_t id) {
    if (!room) {
        return NULL;
    }

    Datastore* datastore = room->parentDatastore;
    if (findNodeByID(datastore, id)) {
        return NULL;
    }

    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        // Memory allocation failed
        return NULL;
    }

    list_element* elem = listInsert(room->nodes, node, NULL);
    if (elem == NULL) {
        // Insertion failed
        free(node);
        return NULL;
    }

    list* sensors = newList();
    if (sensors == NULL) {
        free(node);
        listRemove(room->nodes, elem);
        return NULL;
    }

    list* actuators = newList();
    if (actuators == NULL) {
        free(node);
        deleteList(sensors);
        listRemove(room->nodes, elem);
        return NULL;
    }

    node->id = id;
    node->parentRoom = room;
    node->listPtr = elem;
    node->sensors = sensors;
    node->actuators = actuators;

    return node;
}

Actuator* createActuator (Node* node, uint16_t id, uint8_t type, Position* pos) {
    if (!node) {
        return NULL;
    }

    Pixel* pixel = createPixel(node->parentRoom->parentDatastore, NULL, pos);
    if (!pixel) {
        return NULL;
    }

    if (findActuatorByID(node, id)) {
        // There is already a Actuator with this ID assigned to this node
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

    actuator->type = type;
    actuator->parentNode = node;
    actuator->listPtr = elem;
    actuator->pixel = pixel;

    return actuator;
}

Sensor* createSensor (Node* node, uint8_t type, Position* pos) {
    if (!node) {
        return NULL;
    }

    if (!isValidSensorType(type)) {
        return NULL;
    }

    if (findSensorByType(node, type)) {
        // There's alreay a sensor with the specified type associated with this node.
        return NULL;
    }

    Pixel* pixel = createPixel(node->parentRoom->parentDatastore, NULL, pos);
    if (!pixel) {
        return NULL;
    }

    Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
    if (sensor == NULL) {
        // Memory allocation failed
        return NULL;
    }

    list_element* elem = listInsert(node->sensors, sensor, NULL);
    if (elem == NULL) {
        // Insertion failed
        free(sensor);
        return NULL;
    }

    sensor->parentNode = node;
    sensor->listPtr = elem;
    sensor->type = type;
    sensor->calculator = sensorCalculatorFunctionPointer(type);
    sensor->value = 0;
    sensor->pixel = pixel;

    return sensor;
}

Pixel* createPixel (Datastore* datastore, Color* color, Position* pos) {
    if (!datastore || !pos) {
        return NULL;
    }

    if (findPixelByPos(datastore, pos)) {
        return NULL;
    }

    Color* pixelColor = (Color*)malloc(sizeof(Color));
    if (pixelColor == NULL) {
        // Memory allocation failed
        return NULL;
    }

    if (color) {
        pixelColor->r = color->r;
        pixelColor->g = color->g;
        pixelColor->b = color->b;
    }
    else {
        pixelColor->r = 0;
        pixelColor->g = 0;
        pixelColor->b = 0;
    }


    Position* pixelPos = (Position*)malloc(sizeof(Position));
    if (pixelPos == NULL) {
        // Memory allocation failed
        free(pixelColor);
        return NULL;
    }

    pixelPos->x = pos->x;
    pixelPos->y = pos->y;


    Pixel* pixel = (Pixel*)malloc(sizeof(Pixel));
    if (pixel == NULL) {
        free(pixelColor);
        free(pixelPos);
        // Memory allocation failed
        return NULL;
    }

    list_element* elem = listInsert(datastore->pixels, pixel, NULL);
    if (elem == NULL) {
        // Insertion failed
        free(pixelColor);
        free(pixelPos);
        free(pixel);
        return NULL;
    }

    pixel->color = pixelColor;
    pixel->pos = pixelPos;
    pixel->listPtr = elem;
    pixel->parentDatastore = datastore;

    return pixel;
}

bool deletePixel (Pixel* pixel) {
    if (!pixel) {
        return true;
    }

    list_element* elem = pixel->listPtr;
    Datastore* datastore = pixel->parentDatastore;

    free(pixel->color);
    free(pixel->pos);
    free(pixel);

    list_element* res = listRemove(datastore->pixels, elem);
    if (res == NULL && listSize(datastore->pixels)) {
        return true;
    }

    return false;
}

bool deleteActuator (Actuator* actuator) {
    if (actuator == NULL) {
        return 1;
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

bool deleteSensor (Sensor* sensor) {
    if (sensor == NULL) {
        return 1;
    }

    list_element* elem = sensor->listPtr;
    Node* node = sensor->parentNode;

    deletePixel(sensor->pixel);

    free(sensor);
    list_element* res = listRemove(node->sensors, elem);
    if (res == NULL && listSize(node->sensors)) {
        return 1;
    }

    return 0;
}

bool deleteNode (Node* node) {
    if (node == NULL) {
        return 1;
    }

    list_element* elem = node->listPtr;
    Room* room = node->parentRoom;

    list_element* aux;


    // Delete all node's sensors
    aux = listStart(node->sensors);
    while (aux != NULL) {
        if (deleteSensor(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(node->sensors);
    }
    deleteList(node->sensors);

    // Delete all node's actuators
    aux = listStart(node->actuators);
    while (aux != NULL) {
        if (deleteActuator(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(node->actuators);
    }
    deleteList(node->actuators);

    free(node);
    list_element* res = listRemove(room->nodes, elem);
    if (res == NULL && listSize(room->nodes)) {
        return 1;
    }

    return 0;
}

bool deleteRoom (Room* room) {
    if (room == NULL) {
        return 1;
    }

    list_element* elem = room->listPtr;
    Datastore* datastore = room->parentDatastore;

    list_element* aux;

    // Delete all room's nodes
    aux = listStart(room->nodes);
    while (aux != NULL) {
        if (deleteNode(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(room->nodes);
    }
    deleteList(room->nodes);

    free(room->name);
    free(room);
    list_element* res = listRemove(datastore->rooms, elem);
    if (res == NULL && listSize(datastore->rooms)) {
        return 1;
    }

    return 0;
}

bool deleteDatastore (Datastore* datastore) {
    if (datastore == NULL) {
        return 1;
    }

    list_element* aux;

    // Delete all datastore's rooms
    aux = listStart(datastore->rooms);
    while (aux != NULL) {
        if (deleteRoom(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(datastore->rooms);
    }
    deleteList(datastore->rooms);

    // Delete all datastore's pixels
    aux = listStart(datastore->pixels);
    while (aux != NULL) {
        if (deletePixel(aux->ptr)) {
            // Error
            return 1;
        }
        aux = listStart(datastore->pixels);
    }
    deleteList(datastore->pixels);

    free(datastore);

    return 0;
}

bool setNodeID (Node* node, uint16_t id) {
    if (node == NULL) {
        return 1;
    }

    if (findNodeByID(node->parentRoom->parentDatastore, id)) {
        return 1;
    }

    node->id = id;

    return 0;
}

bool setRoomID (Room* room, uint16_t id) {
    if (room == NULL) {
        return 1;
    }

    if (findRoomByID(room->parentDatastore, id)) {
        return 1;
    }

    room->id = id;

    return 0;
}

bool setRoomName (Room* room, const char* str) {
    if (room == NULL || str == NULL) {
        return 1;
    }

    if (findRoomByName(room->parentDatastore, str)) {
        return 1;
    }

    room->name = (char*)realloc(room->name, strlen(str)+1);
    strcpy(room->name, str);

    return 0;
}

bool setSensorValue (Sensor* sensor, uint16_t value) {
    if (!sensor) {
        return 1;
    }

    sensor->value = value;
    
    return 0;
}

float getSensorValue (Sensor* sensor) {
    if (sensor == NULL) {
        return 0;
    }

    return (sensor->calculator)(sensor->value);
}

Pixel* getActuatorPixel (Actuator* actuator) {
    if (!actuator) {
        return NULL;
    }

    return actuator->pixel;
}

Pixel* getSensorPixel (Sensor* sensor) {
    if (!sensor) {
        return NULL;
    }

    return sensor->pixel;
}

Position* getPixelPosition (Pixel* pixel) {
    if (!pixel) {
        return NULL;
    }

    return pixel->pos;
}

Color* getPixelColor (Pixel* pixel) {
    if (!pixel) {
        return NULL;
    }

    return pixel->color;
}

Node* findNodeByID (Datastore* datastore, uint16_t nodeID) {
    if (!datastore) {
        return NULL;
    }

    for (list_element* room_elem = listStart(datastore->rooms); room_elem != NULL; room_elem = room_elem->next) {
        Room* room = (Room*)room_elem->ptr;
        for (list_element* node_elem = listStart(room->nodes); node_elem != NULL; node_elem = node_elem->next) {
            Node* node = (Node*)node_elem->ptr;
            if (node->id == nodeID) {
                return node;
            }
        }
    }

    return NULL;
}

Room* findRoomByID (Datastore* datastore, uint16_t roomID) {
    if (!datastore) {
        return NULL;
    }

    for (list_element* room_elem = listStart(datastore->rooms); room_elem != NULL; room_elem = room_elem->next) {
        Room* room = (Room*)room_elem->ptr;
        if (room->id == roomID) {
            return room;
        }
    }

    return NULL;
}

Actuator* findActuatorByID (Node* node, uint16_t actuatorID) {
    if (!node) {
        return NULL;
    }

    for (list_element* actuator_elem = listStart(node->actuators); actuator_elem != NULL; actuator_elem = actuator_elem->next) {
        Actuator* actuator = (Actuator*)actuator_elem->ptr;
        if (actuator->id == actuatorID) {
            return actuator;
        }
    }

    return NULL;
}

Room* findRoomByName (Datastore* datastore, const char* roomName) {
    if (!datastore || !roomName) {
        return NULL;
    }
    for (list_element* room_elem = listStart(datastore->rooms); room_elem != NULL; room_elem = room_elem->next) {
        Room* room = (Room*)room_elem->ptr;
        if (!room->name) {
            continue;
        }
        if (!strcmp(room->name, roomName)) {
            return room;
        }
    }

    return NULL;
}

Pixel* findPixelByPos (Datastore* datastore, Position* pos) {
    if (!datastore || !pos) {
        return NULL;
    }

    LL_iterator(datastore->pixels, pixel_elem) {
        Pixel* pixel = pixel_elem->ptr;
        if (pixel->pos->x == pos->x && pixel->pos->y == pos->y) {
            return pixel;
        }
    }

    return NULL;
}

Sensor* findSensorByType (Node* node, uint8_t type) {
    if (!node) {
        return NULL;
    }

    for (list_element* elem = listStart(node->sensors); elem != NULL; elem = elem->next) {
        Sensor* sensor = elem->ptr;
        if (sensor->type == type) {
            // There's already a sensor of this type registered on to this node
            return sensor;
        }
    }

    return NULL;
}

bool iterateActuators (Datastore* datastore, bool (*func)(Actuator*)) {
    if (!datastore || !func) {
        return NULL;
    }

    // For every Room
    for (list_element* room_elem = listStart(datastore->rooms); room_elem != NULL; room_elem = room_elem->next) {
        Room* room = (Room*)room_elem->ptr;

        // For every Node
        for (list_element* node_elem = listStart(room->nodes); node_elem != NULL; node_elem = node_elem->next) {
            Node* node = (Node*)node_elem->ptr;
            
            // For every Actuator
            for (list_element* actuator_elem = listStart(node->actuators); actuator_elem != NULL; actuator_elem = actuator_elem->next) {
                Actuator* actuator = (Actuator*)actuator_elem->ptr;
                if ((*func)(actuator)) {
                    return true;
                }
            }
        }
    }

    return false;
}