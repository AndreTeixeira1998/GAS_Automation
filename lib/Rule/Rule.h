#ifndef __RULE__
#define __RULE__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LinkedList.h"

typedef struct _rule Rule;

#include "Sensor.h"
#include "Actuator.h"
#include "Profile.h"

#define TYPE_RULE_LESS_THEN     0
#define TYPE_RULE_GREATER_THEN  1
#define TYPE_RULE_EQUAL_TO      2
#define TYPE_RULE_WITHIN_MARGIN 3


/**
 * @brief Structure to hold a rule for actuator control
 * 
 */
struct _rule {
    Datastore* parentDatastore;
    Rule* parentRule;
    uint16_t id;
    list_element* listPtr;
    list_element* listPtr_parentRule;
    list* sensors;
    list* actuators;
    uint16_t operation;
    uint16_t value;
    list* childs;
    list* profiles;
};

/**
 * @brief Create a Rule object
 * 
 * @param datastore Datastore associated with the rule
 * @param parent Parent Rule
 * @param id ID of the new rule. Must be globaly unique
 * @param type operation to be made to the value of associated sensors and the rule value
 * @param value 
 * @return Rule* Pointer to the new Rule object
 */
Rule* createRule (Datastore* datastore, Rule* parentRule, uint16_t id, uint16_t type, uint16_t value);

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
bool executeRules (Datastore* datastore, bool uploadValues, list* queryList);

/**
 * @brief Search the datastore for a Rule with the specified ID
 * 
 * @param datastore Pointer to the Datastore object to be searched
 * @param id ID of the Rule to find
 * @return Rule* Rule object with specified ID. NULL if error or not found.
 */
Rule* findRuleByID (Datastore* datastore, uint16_t id);

/**
 * @brief Adds the profile to the rule
 * 
 * @param rule Rule object
 * @param profile Profile Object
 * @return true Error
 * @return false All good
 */
bool addProfileToRule (Rule* rule, Profile* profile);

/**
 * @brief Removes the profile from the rule
 * 
 * @param rule Rule object
 * @param profile Profile Object
 * @return true Error
 * @return false All good
 */
bool removeProfileFromRule (Rule* rule, Profile* profile);

void prepareRuleQueries (list* queryList);
void preparePriorityRuleQueries (list* queryList);

#endif
