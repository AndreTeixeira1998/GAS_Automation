#ifndef __PROFILE__
#define __PROFILE__

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef struct _profile Profile;

#include "Datastore.h"


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

#endif
