#ifndef __DATASTRUCTURE__
#define __DATASTRUCTURE__

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"

#include "LinkedList.h"

#define NAME_MAX_LENGTH 24

#define N_TYPE_SENSOR           5
#define TYPE_SENSOR_VOLTAGE     0
#define TYPE_SENSOR_TEMPERATURE 1
#define TYPE_SENSOR_HUMIDITY    2
#define TYPE_SENSOR_LIGHT       3
#define TYPE_SENSOR_CURRENT     4

/**
 * @brief Structure to hold all data concerning an actuator.
 * 
 */
typedef struct {
    struct Node* parentNode;
    list_element* listPtr;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Actuator;

/**
 * @brief "category" of functions used to calculate the value of physical parameters from the raw sensor data.
 * 
 * @param value 
 * @return typedef sensorValueCalculator 
 */
typedef float sensorValueCalculator(uint16_t value);

/**
 * @brief Structure to hold all data concerning a sensor.
 * 
 */
typedef struct {
    struct Node* parentNode;
    list_element* listPtr;
    uint8_t type;
    sensorValueCalculator* calculator;
    uint16_t value;
} Sensor;

/**
 * @brief Structure to hold all data concerning a Node.
 * 
 */
typedef struct {
    uint16_t id;
    struct Room* parentRoom;
    list_element* listPtr;
    list* sensors;
    list* actuators;
} Node;

/**
 * @brief Structure to hold all data concerning a Room.
 * 
 */
typedef struct {
    uint16_t id;
    struct Datastore* parentDatastore;
    list_element* listPtr;
    char* name;
    list* nodes;
} Room;

/**
 * @brief Main structure that stores all data concerning a space.
 * 
 */
typedef struct {
    list* rooms;
} Datastore;



/**
 * @brief Calculates the voltage a sensor is mesuring from it raw data.
 * 
 * @param value Raw 16bit value
 * @return float result in Volts
 */
float voltageSensorValue (uint16_t value);

/**
 * @brief Calculates the temperature a sensor is mesuring from it raw data.
 * 
 * @param value Raw 16bit value
 * @return float result in degrees Celsius
 */
float temperatureSensorValue (uint16_t value);

/**
 * @brief Calculates the relative humidity a sensor is mesuring from it raw data.
 * 
 * @param value Raw 16bit value
 * @return float result in percentage
 */
float humiditySensorValue (uint16_t value);

/**
 * @brief Calculates the light intensity a sensor is mesuring from it raw data.
 * 
 * @param value Raw 16bit value
 * @return float result in Lux
 */
float lightSensorValue (uint16_t value);

/**
 * @brief Calculates the current a sensor is mesuring from it raw data.
 * 
 * @param value Raw 16bit value
 * @return float result in Ampère
 */
float currentSensorValue (uint16_t value);

#define TYPE_SENSOR_VOLTAGE_VALUE_CALCULATOR        (&voltageSensorValue)
#define TYPE_SENSOR_TEMPERATURE_VALUE_CALCULATOR    (&temperatureSensorValue)
#define TYPE_SENSOR_HUMIDITY_VALUE_CALCULATOR       (&humiditySensorValue)
#define TYPE_SENSOR_LIGHT_VALUE_CALCULATOR          (&lightSensorValue)
#define TYPE_SENSOR_CURRENT_VALUE_CALCULATOR        (&currentSensorValue)

#define VALUE_CALCULATOR(x) (x ## _VALUE_CALCULATOR)

/**
 * @brief Determines if a type is valid
 * 
 * @param type Sensor type
 * @return true Valid type
 * @return false Invalid type
 */
bool isValidType (uint8_t type);

/**
 * @brief Create a Datastore object
 * 
 * @return Datastore* The pointer to the new Datastore object. NULL if error occurs.
 */
Datastore* createDatastore ();

/**
 * @brief Create a Room object
 * 
 * @param datastore Datastore in which to be inserted.
 * @return Room* The pointer to the new Room object. NULL if error occurs.
 */
Room* createRoom (Datastore* datastore);

/**
 * @brief Create a Node object
 * 
 * @param room A pointer to the Room this Node will belong to.
 * @return Node* The pointer to the new Node object. NULL if error occurs.
 */
Node* createNode (Room* room);

/**
 * @brief Create a Actuator object
 * 
 * @param node A pointer to the Node this Actuator will belong to.
 * @return Actuator* The pointer to the new Actuator object. NULL if error occurs.
 */
Actuator* createActuator (Node* node);

/**
 * @brief Create a Sensor object
 * 
 * @param node A pointer to the Node this Sensor will belong to.
 * @return Sensor* The pointer to the new Sensor object. NULL if error occurs.
 */
Sensor* createSensor (Node* node, uint8_t type);


/**
 * @brief Delete a Actuator object
 * 
 * @param actuator The pointer to the Actuator object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteActuator (Actuator* actuator);

/**
 * @brief Delete a Sensor object
 * 
 * @param sensor The pointer to the Sensor object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteSensor (Sensor* sensor);

/**
 * @brief Delete a Node object and all it's childs
 * 
 * @param node The pointer to the Node object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteNode (Node* node);

/**
 * @brief Delete a Room object and all it's childs
 * 
 * @param room The pointer to the Room object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteRoom (Room* room);

/**
 * @brief Delete a Datastore object and all it's childs
 * 
 * @param datastore The pointer to the Datastore object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteDatastore (Datastore* datastore);

/**
 * @brief Set the Node ID object
 * 
 * @param node Pointer to the Node object
 * @param id 
 * @return true Error
 * @return false All good
 */
bool setNodeID (Node* node, uint16_t id);

/**
 * @brief Set the Room ID object
 * 
 * @param room Pointer to the Room object
 * @param id 
 * @return true Error
 * @return false All good
 */
bool setRoomID (Room* room, uint16_t id);

/**
 * @brief Set the Room Name object
 * 
 * @param room Pointer to the Room object
 * @param str Room name
 * @return true Error
 * @return false All good
 */
bool setRoomName(Room* room, char* str);

/**
 * @brief Calculate the value of the Sensor object
 * 
 * @param sensor Pointer to the sendor object
 * @return float Value of the sensor. 0 in case of error
 */
float getSensorValue (Sensor* sensor);

#endif