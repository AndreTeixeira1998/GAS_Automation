#ifndef __RULE__
#define __RULE__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LinkedList.h"

#include "Sensor.h"
#include "Actuator.h"

#define TYPE_RULE_LESS_THEN     0
#define TYPE_RULE_GREATER_THEN  1
#define TYPE_RULE_EQUAL_TO      2
#define TYPE_RULE_WITHIN_MARGIN 3

typedef struct _rule Rule;

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
