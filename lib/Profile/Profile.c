#include "Profile.h"

Profile* createProfile (Datastore* datastore, uint16_t id) {
    if (!datastore) {
        return NULL;
    }

    if (findProfileByID(datastore, id)) {
        // There's already a profile with the specified ID
        return NULL;
    }

    // Alocate memory for this profile
    Profile* profile = (Profile*)malloc(sizeof(Profile));
    if (!profile) {
        return NULL;
    }

    // Fill Profile data
    profile->id = id;

    // Insert profile in the datastore
    list_element* elem = listInsert(datastore->profiles, profile, NULL);

    if (!elem) {
        // Insertion failed
        free(profile);
        return NULL;
    }

    profile->listPtr = elem;

    return profile;
}

bool deleteProfile (Profile* profile) {
    if (!profile) {
        return true;
    }

    LL_iterator(profile->parentDatastore->rules, rule_elem) {
        Rule* rule = (Rule*)rule_elem->ptr;
        if (removeProfileFromRule(rule, profile)) {
            return true;
        }
    }


    list_element *elem = profile->listPtr,
        *res;

    res = listRemove(profile->parentDatastore->profiles, elem);
    if (res == NULL && listSize(profile->parentDatastore->profiles)) {
        // TODO check this for logic error
        return true;
    }


    free(profile);

    return false;
}

Profile* findProfileByID (Datastore* datastore, uint16_t id) {
    if (!datastore) {
        return NULL;
    }

    LL_iterator(datastore->profiles, profile_elem) {
        Profile* profile = (Profile*)profile_elem->ptr;
        if (profile->id == id) {
            return profile;
        }
    }

    return NULL;
}
