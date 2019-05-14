#ifndef __PROFILE__
#define __PROFILE__

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct _profile Profile;

#include "Datastore.h"
#include "DBLink.h"


/**
 * @brief Main structure that stores all data concerning a space.
 * 
 */
struct _profile {
    Datastore* parentDatastore;
    list_element* listPtr;
    uint16_t id;
};


/**
 * @brief Create a Profile object
 * 
 * @param datastore Datastore to add the profile to
 * @param id ID of the Profile. Must be globaly unique
 * @return Profile* Pointer to the Profile object
 */
Profile* createProfile (Datastore* datastore, uint16_t id);

/**
 * @brief Delete a Profile object and remove it from all rules it is aplied to
 * 
 * @param profile Pointer to Profile object
 * @return true Error
 * @return false All good
 */
bool deleteProfile (Profile* profile);

/**
 * @brief Find a Profile with the given ID
 * 
 * @param datastore Pointer to Datastore object for where to search for the Profile
 * @param id ID of the Profile to search for
 * @return Profile* Pointer to the Profile object
 */
Profile* findProfileByID (Datastore* datastore, uint16_t id);


/**********************************/
/*        DATABASE QUERIES        */
/**********************************/
/*
#define PROFILE_QUERY_ARRAY_SIZE 3 // Change to match number of nodes below

DBQuery create_table_profile = {
    NULL,
    "create_table_profile",
    "CREATE TABLE IF NOT EXISTS sinf.profile("
    "profiles_id SERIAL NOT NULL PRIMARY KEY,"
    "start_date TIMESTAMP,"
    "end_date TIMESTAMP);",
    0
};

DBQuery create_profile = {
    NULL,
    "create_profile",
    "INSERT INTO sinf.profile(start_date, end_date) "
    "VALUES($1, $2);",
    2
};

DBQuery delete_profile = {
    NULL,
    "delete_profile",
    "DELETE FROM sinf.profile WHERE profile_id=$1;",
    1
};

DBQuery* profileQueryArray[PROFILE_QUERY_ARRAY_SIZE] = {
    &create_table_profile,
    &create_profile,
    &delete_profile
};
*/
#endif
