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

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_profile = {
    NULL,
    "create_table_profile",
    "CREATE TABLE IF NOT EXISTS sinf.profile("
    "profile_id SERIAL NOT NULL PRIMARY KEY,"
    "start_date TIME,"
    "end_date TIME);",
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

void prepareProfileQueries (list* queryList) {
    addQuerytoList(&create_table_profile, queryList);
    addQuerytoList(&create_profile, queryList);
    addQuerytoList(&delete_profile, queryList);
}