#include "Sensor.h"

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

Sensor* createSensor (Node* node, uint16_t id, uint8_t type, Position* pos, uint16_t rangeMin, uint16_t rangeMax) {
    if (!node) {
        return NULL;
    }

    if (!isValidSensorType(type)) {
        return NULL;
    }

    Room* room = (Room*)node->parentRoom;
    Datastore* datastore = (Datastore*)room->parentDatastore;
    if (findSensorByID(datastore, id)) {
        // There's alreay a sensor with the specified ID.
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
        deletePixel(pixel);
        return NULL;
    }

    if(pthread_mutex_init(&sensor->mutex, NULL)) {
        // Mutex init failed
        deletePixel(pixel);
        return NULL;
    }

    list_element* elem = listInsert(node->sensors, sensor, NULL);
    if (elem == NULL) {
        // Insertion failed
        pthread_mutex_destroy(&sensor->mutex);
        deletePixel(pixel);
        free(sensor);
        return NULL;
    }

    sensor->parentNode = node;
    sensor->listPtr = elem;
    sensor->id = id;
    sensor->type = type;
    sensor->calculator = sensorCalculatorFunctionPointer(type);
    sensor->value = 0;
    sensor->pixel = pixel;
    sensor->rangeMin = rangeMin;
    sensor->rangeMax = rangeMax;

    return sensor;
}

bool deleteSensor (Sensor* sensor) {
    if (sensor == NULL) {
        return 1;
    }

    // Remove Sensor from existing rules
    Datastore* datastore = sensor->parentNode->parentRoom->parentDatastore;
    LL_iterator(datastore->rules, rule_elem) {
        Rule* rule = rule_elem->ptr;
        LL_iterator(rule->sensors, ruleSensor_elem) {
            if (sensor == (ruleSensor_elem->ptr)) {
                list_element* rule_remover = listRemove(rule->sensors, ruleSensor_elem);
                if (rule_remover == NULL && listSize(rule->sensors)) {
                    return 1;
                }
                break;
            }
        }
    }

    list_element* elem = sensor->listPtr;
    Node* node = sensor->parentNode;
    pthread_mutex_destroy(&sensor->mutex);

    deletePixel(sensor->pixel);

    free(sensor);
    list_element* res = listRemove(node->sensors, elem);
    if (res == NULL && listSize(node->sensors)) {
        return 1;
    }

    return 0;
}

bool setSensorValue (Sensor* sensor, uint16_t value) {
    if (!sensor) {
        return 1;
    }

    pthread_mutex_lock(&sensor->mutex);
    sensor->value = value;
    pthread_mutex_unlock(&sensor->mutex);
    
    return 0;
}

float getSensorValue (Sensor* sensor) {
    if (sensor == NULL) {
        return 0;
    }

    pthread_mutex_lock(&sensor->mutex);
    float res = (sensor->calculator)(sensor->value);
    pthread_mutex_unlock(&sensor->mutex);

    return res;
}

Pixel* getSensorPixel (Sensor* sensor) {
    if (!sensor) {
        return NULL;
    }

    return sensor->pixel;
}

Sensor* findSensorByType (Node* node, uint8_t type) {
    if (!node) {
        return NULL;
    }

    LL_iterator(node->sensors, sensor_elem) {
        Sensor* sensor = sensor_elem->ptr;
        if (sensor->type == type) {
            // There's already a sensor of this type registered on to this node
            return sensor;
        }
    }

    return NULL;
}

Sensor* findSensorByID (Datastore* datastore, uint16_t id) {
    if (!datastore) {
        return NULL;
    }

    LL_iterator(datastore->rooms, room_elem) {
        Room* room = room_elem->ptr;
        LL_iterator(room->nodes, node_elem) {
            Node* node = node_elem->ptr;
            LL_iterator(node->sensors, sensor_elem) {
                Sensor* sensor = sensor_elem->ptr;
                if (sensor->id == id) {
                    // There's already a sensor registered with this ID
                    return sensor;
                }
            }
        }
    }

    return NULL;
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max) / (in_max - in_min) + out_min;
}

bool updateSensorPixel (Sensor* sensor) {
    if (!sensor) {
        return true;
    }

    Pixel* pixel = getSensorPixel(sensor);
    if (!pixel) {
        return true;
    }

    Color* color = getPixelColor(pixel);
    if (!color) {
        return true;
    }

    float sensorValue = getSensorValue(sensor);

    float mappedRed = map(sensorValue, sensor->rangeMin, sensor->rangeMax, 0, 255);
    //printf("Red: %d  :  %d   :   %f   :   %f\n", sensor->type, sensor->value, sensorValue, mappedRed);

    pthread_mutex_lock(&pixel->mutex);
    color->r = 0;
    color->g = 0;
    color->b = (int)mappedRed;
    pthread_mutex_unlock(&pixel->mutex);

    return false;
}

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_sensor = {
    NULL,
    "create_table_sensor",
    "CREATE TABLE IF NOT EXISTS sinf.sensor("
    "sensor_id INTEGER NOT NULL PRIMARY KEY,"
    "type INTEGER NOT NULL,"
    "pixel_id INTEGER NOT NULL,"
    "FOREIGN KEY (pixel_id) REFERENCES sinf.pixel(pixel_id) ON UPDATE CASCADE ON DELETE CASCADE"
    ");",
    0
};

DBQuery create_sensor = {
    NULL,
    "create_sensor",
    "INSERT INTO sinf.sensor(sensor_id, type, pixel_id) "
    "VALUES($1, $2, $3);",
    3
};

DBQuery delete_sensor = {
    NULL,
    "delete_sensor",
    "DELETE FROM sinf.sensor WHERE sensor_id=$1;",
    1
};

DBQuery create_table_node_sensor = {
    NULL,
    "create_table_node_sensor",
    "CREATE TABLE IF NOT EXISTS sinf.node_sensor("
    "node_sensor_id SERIAL NOT NULL PRIMARY KEY,"
    "node_id INTEGER NOT NULL,"
    "sensor_id INTEGER NOT NULL,"
    "start_date TIMESTAMP NOT NULL DEFAULT NOW(),"
    "end_date TIMESTAMP,"
    "FOREIGN KEY (node_id) REFERENCES sinf.node(node_id) ON UPDATE CASCADE ON DELETE CASCADE,"
    "FOREIGN KEY (sensor_id) REFERENCES sinf.sensor(sensor_id) ON UPDATE CASCADE ON DELETE CASCADE);",
    0
};

DBQuery add_sensor_to_node = {
    NULL,
    "add_sensor_to_node",
    "INSERT INTO sinf.node_sensor(node_id, sensor_id) "
    "VALUES($1, $2);",
    2
};

DBQuery remove_sensor_from_node = {
    NULL,
    "remove_sensor_from_node",
    "UPDATE sinf.node_sensor "
    "SET end_date = NOW() "
    "WHERE node_id = $1 AND sensor_id = $2 AND end_date = NULL;",
    2
};

DBQuery create_table_sensor_state = {
    NULL,
    "create_table_sensor_state",
    "CREATE TABLE IF NOT EXISTS sinf.sensor_state("
    "sensor_state_id SERIAL NOT NULL PRIMARY KEY,"
    "sensor_id INTEGER NOT NULL,"
    "value REAL NOT NULL,"
    "timestamp TIMESTAMP NOT NULL DEFAULT NOW(),"
    "FOREIGN KEY (sensor_id) REFERENCES sinf.sensor(sensor_id) ON UPDATE CASCADE ON DELETE CASCADE"
    ");",
    0
};

DBQuery create_sensor_state = {
    NULL,
    "create_sensor_state",
    "INSERT INTO sinf.sensor_state(sensor_id, value) "
    "VALUES($1, $2);",
    2
};

void preparePrioritySensorQueries (list* queryList) {
    addQuerytoList(&create_table_sensor, queryList);
    addQuerytoList(&create_table_node_sensor, queryList);
    addQuerytoList(&create_table_sensor_state, queryList);
}

void prepareSensorQueries (list* queryList) {
    addQuerytoList(&create_sensor, queryList);
    addQuerytoList(&delete_sensor, queryList);
    addQuerytoList(&add_sensor_to_node, queryList);
    addQuerytoList(&remove_sensor_from_node, queryList);
    addQuerytoList(&create_sensor_state, queryList);
}

