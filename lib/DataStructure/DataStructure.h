#ifndef __DATASTRUCTURE__
#define __DATASTRUCTURE__

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "string.h"
#include "pthread.h"

#include "LinkedList.h"

#define NAME_MAX_LENGTH 24

#define N_TYPE_SENSOR           5
#define TYPE_SENSOR_VOLTAGE     0
#define TYPE_SENSOR_TEMPERATURE 1
#define TYPE_SENSOR_HUMIDITY    2
#define TYPE_SENSOR_LIGHT       3
#define TYPE_SENSOR_CURRENT     4

#define TYPE_RULE_LESS_THEN     0
#define TYPE_RULE_GREATER_THEN  1
#define TYPE_RULE_EQUAL_TO      2
#define TYPE_RULE_WITHIN_MARGIN 3

typedef struct _datastore Datastore;
typedef struct _room Room;
typedef struct _node Node;
typedef struct _sensor Sensor;
typedef struct _actuator Actuator;
typedef struct _position Position;
typedef struct _color Color;
typedef struct _rule Rule;
typedef struct _pixel Pixel;

/**
 * @brief Structure to hold a rule for actuator control
 * 
 */
struct _rule {
    Room* parentRoom;
    Rule* parentRule;
    list_element* listPtr;
    list* sensors;
    list* actuators;
    uint16_t operation;
    uint16_t value;
    list* childs;
};

/**
 * @brief Structure to hold a cartesian position
 * 
 */
struct _position {
    uint16_t x;
    uint16_t y;
};

/**
 * @brief Structure to hold a color in RGB format
 * 
 */
struct _color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/**
 * @brief Structure to hold all data regarding a pixel in the RGB Matrix
 * 
 */
struct _pixel {
    Position* pos;
    Color* color;
    list_element* listPtr;
    Datastore* parentDatastore;
    pthread_mutex_t mutex;
};

/**
 * @brief Structure to hold all data concerning an actuator.
 * 
 */
struct _actuator {
    Node* parentNode;
    list_element* listPtr;
    uint16_t id;
    uint8_t type;
    Pixel* pixel;
};

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
struct _sensor {
    Node* parentNode;
    list_element* listPtr;
    uint8_t type;
    sensorValueCalculator* calculator;
    uint16_t value;
    Pixel* pixel;
    pthread_mutex_t mutex;
};

/**
 * @brief Structure to hold all data concerning a Node.
 * 
 */
struct _node {
    uint16_t id;
    Room* parentRoom;
    list_element* listPtr;
    list* sensors;
    list* actuators;
};

/**
 * @brief Structure to hold all data concerning a Room.
 * 
 */
struct _room {
    uint16_t id;
    Datastore* parentDatastore;
    list_element* listPtr;
    char* name;
    list* nodes;
    list* rules;
};

/**
 * @brief Main structure that stores all data concerning a space.
 * 
 */
struct _datastore {
    list* rooms;
    list* pixels;
};



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
bool isValidSensorType (uint8_t type);

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
 * @param id The ID for the new Room. Must be unique.
 * @return Room* The pointer to the new Room object. NULL if error occurs.
 */
Room* createRoom (Datastore* datastore, uint16_t id);

/**
 * @brief Create a Node object
 * 
 * @param room A pointer to the Room this Node will belong to.
 * @param id The ID of the new Node. Must be unique.
 * @return Node* The pointer to the new Node object. NULL if error occurs.
 */
Node* createNode (Room* room, uint16_t id);

/**
 * @brief Create a Actuator object
 * 
 * @param node A pointer to the Node this Actuator will belong to.
 * @param id The ID of the new Actuator. Must be unique inside the Node.
 * @param type Type of the Actuator
 * @param posX Position in the X axis on the RGB Matrix output
 * @param posY Position in the Y axis on the RGB Matrix output
 * @return Actuator* The pointer to the new Actuator object. NULL if error occurs.
 */
Actuator* createActuator (Node* node, uint16_t id, uint8_t type, Position* pos);

/**
 * @brief Create a Sensor object
 * 
 * @param node A pointer to the Node this Sensor will belong to.
 * @param type Type of sensor
 * @param pos Position of the sensor in the RGB Matrix
 * @return Sensor* The pointer to the new Sensor object. NULL if error occurs.
 */
Sensor* createSensor (Node* node, uint8_t type, Position* pos);

/**
 * @brief Create a Pixel object
 * 
 * @param datastore Datastore to save the Pixel
 * @param color Color of the pixel
 * @param pos Position of the Pixel
 * @return Pixel* Pointer to the new Pixel object. NULL if error.
 */
Pixel* createPixel (Datastore* datastore, Color* color, Position* pos);

/**
 * @brief Create a Rule object
 * 
 * @param room Room associated with the rule
 * @param parent Parent Rule
 * @param type operation to be made to the value of associated sensors and the rule value
 * @param value 
 * @return Rule* Pointer to the new Rule object
 */
Rule* createRule (Room* room, Rule* parent, uint16_t type, uint16_t value);

/**
 * @brief Delete a Rule object
 * 
 * @param rule The pointer to the Rule object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteRule (Rule* rule);

/**
 * @brief Delete a Pixel object
 * 
 * @param actuator The pointer to the Pixel object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deletePixel (Pixel* pixel);

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
bool setRoomName(Room* room, const char* str);

/**
 * @brief Set the raw value of the Sensor object
 * 
 * @param sensor Pointer to the Sensor object
 * @return true Error
 * @return false All Good
 */
bool setSensorValue (Sensor* sensor, uint16_t value);

/**
 * @brief Set the Pixel Position object
 * 
 * @param pixel Pointer to the Pixel object
 * @param pos Pointer to a Position object with the desired specs
 * @return true Error
 * @return false All good
 */
bool setPixelPosition (Pixel* pixel, Position* pos);

/**
 * @brief Set the Pixel Color object
 * 
 * @param pixel Pointer to the Pixel object
 * @param color Pointer to a Color object with the desired specs
 * @return true Error
 * @return false All good
 */
bool setPixelColor (Pixel* pixel, Color* color);

/**
 * @brief Calculate the value of the Sensor object
 * 
 * @param sensor Pointer to the Sensor object
 * @return float Value of the sensor. 0 in case of error
 */
float getSensorValue (Sensor* sensor);

/**
 * @brief Get the Actuator Pixel object
 * 
 * @param actuator Pointer to the Actuator object
 * @return Pixel* Pointer to the Pixel object. NULL if error.
 */
Pixel* getActuatorPixel (Actuator* actuator);

/**
 * @brief Get the Sensor Pixel object
 * 
 * @param actuator Pointer to the Sensor object
 * @return Pixel* Pointer to the Pixel object. NULL if error.
 */
Pixel* getSensorPixel (Sensor* sensor);

/**
 * @brief Get the Pixel Position object
 * 
 * @param pixel Pointer to the Pixel object
 * @return Position* Pointer to the Position of the Pixel. NULL if error.
 */
Position* getPixelPosition (Pixel* pixel);

/**
 * @brief Get the Pixel Color object
 * 
 * @param actuator Pointer to the Pixel object
 * @return Color* Pointer to the Color of the Pixel. NULL if error.
 */
Color* getPixelColor (Pixel* pixel);

/**
 * @brief Searches the datastore for a Node with the specified nodeID
 * 
 * @param datastore Datastore object to search
 * @param nodeID nodeID to find
 * @return Node* Node object with 'nodeID' id. NULL if not found.
 */
Node* findNodeByID (Datastore* datastore, uint16_t nodeID);

/**
 * @brief Searches the datastore for a Room with the specified roomID
 * 
 * @param datastore Datastore object to search
 * @param roomID roomID to find
 * @return Room* Room object with 'roomID' id. NULL if not found.
 */
Room* findRoomByID (Datastore* datastore, uint16_t roomID);

/**
 * @brief Searches the node for a Actuator with the specified actuatorID
 * 
 * @param node Node object to search
 * @param actuatorID ID of the actuator to find
 * @return Actuator* Actuator object with 'actuatorID' id. NULL if not found.
 */
Actuator* findActuatorByID (Node* node, uint16_t actuatorID);

/**
 * @brief Searches the datastore for a Room with the specified roomName
 * 
 * @param datastore Datastore object to search
 * @param roomName roomName to find
 * @return Room* Room object with 'roomName' name. NULL if not found.
 */
Room* findRoomByName (Datastore* datastore, const char* roomName);

/**
 * @brief Searches the datastore for a Pixel with the specified position
 * 
 * @param datastore Datastore object to search
 * @param pos Position on the RGB Matrix output
 * @return Pixel* Pixel object with the desired position. NULL if not found.
 */
Pixel* findPixelByPos (Datastore* datastore, Position* pos);

/**
 * @brief Searches a node's sensors for the one with a specific type.
 * 
 * @param node Node to search
 * @param type type of sensor to find
 * @return Sensor* sensor object with specified type. NULL if error or not found.
 */
Sensor* findSensorByType (Node* node, uint8_t type);

/**
 * @brief Adds a Sensor as a condition to a rule
 * 
 * @param rule 
 * @param sensor 
 * @return true Error
 * @return false All Good
 */
bool addSensorToRule (Rule* rule, Sensor* sensor);

/**
 * @brief Adds a Actuator as a condition to a rule
 * 
 * @param rule 
 * @param actuator 
 * @return true Error
 * @return false All Good
 */
bool addActuatorToRule (Rule* rule, Actuator* actuator);

/**
 * @brief Execute the control rules
 * 
 * @param datastore 
 * @return true Error
 * @return false All Good
 */
bool executeRules (Datastore* datastore);

#endif