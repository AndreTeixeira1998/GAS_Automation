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

Sensor* createSensor (Node* node, uint8_t type, Position* pos, uint16_t rangeMin, uint16_t rangeMax) {
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
    LL_iterator(datastore->rooms, room_elem) {
        Room* room = room_elem->ptr;
        LL_iterator(room->rules, rule_elem) {
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

    for (list_element* elem = listStart(node->sensors); elem != NULL; elem = elem->next) {
        Sensor* sensor = elem->ptr;
        if (sensor->type == type) {
            // There's already a sensor of this type registered on to this node
            return sensor;
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
