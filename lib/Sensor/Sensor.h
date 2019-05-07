#ifndef __SENSOR__
#define __SENSOR__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#include "LinkedList.h"

typedef struct _sensor Sensor;

#include "Pixel.h"
#include "Rule.h"
#include "Node.h"
#include "Position.h"

#define N_TYPE_SENSOR           5
#define TYPE_SENSOR_VOLTAGE     0
#define TYPE_SENSOR_TEMPERATURE 1
#define TYPE_SENSOR_HUMIDITY    2
#define TYPE_SENSOR_LIGHT       3
#define TYPE_SENSOR_CURRENT     4

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
    uint16_t rangeMin;
    uint16_t rangeMax;
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
 * @brief Create a Sensor object
 * 
 * @param node A pointer to the Node this Sensor will belong to.
 * @param type Type of sensor
 * @param pos Position of the sensor in the RGB Matrix
 * @param rangeMin min value for color range
 * @param rangeMax max value for color range
 * @return Sensor* The pointer to the new Sensor object. NULL if error occurs.
 */
Sensor* createSensor (Node* node, uint8_t type, Position* pos, uint16_t rangeMin, uint16_t rangeMax);

/**
 * @brief Delete a Sensor object
 * 
 * @param sensor The pointer to the Sensor object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deleteSensor (Sensor* sensor);

/**
 * @brief Set the raw value of the Sensor object
 * 
 * @param sensor Pointer to the Sensor object
 * @return true Error
 * @return false All Good
 */
bool setSensorValue (Sensor* sensor, uint16_t value);

/**
 * @brief Calculate the value of the Sensor object
 * 
 * @param sensor Pointer to the Sensor object
 * @return float Value of the sensor. 0 in case of error
 */
float getSensorValue (Sensor* sensor);

/**
 * @brief Get the Sensor Pixel object
 * 
 * @param sensor Pointer to the Sensor object
 * @return Pixel* Pointer to the Pixel object. NULL if error.
 */
Pixel* getSensorPixel (Sensor* sensor);

/**
 * @brief Searches a node's sensors for the one with a specific type.
 * 
 * @param node Node to search
 * @param type type of sensor to find
 * @return Sensor* sensor object with specified type. NULL if error or not found.
 */
Sensor* findSensorByType (Node* node, uint8_t type);

/**
 * @brief Update pixel color for sensors
 * 
 * @param sensor Pointer to Sensor Object
 * @return true Error
 * @return false All good
 */
bool updateSensorPixel (Sensor* sensor);

#endif
