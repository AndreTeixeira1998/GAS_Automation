#include "Profile.h"

Profile* createProfile (Datastore* datastore, uint16_t id, const char* name, const char* start, const char* end) {
    if (!datastore) {
        return NULL;
    }

    if (findProfileByID(datastore, id)) {
        // There's already a profile with the specified ID
        return NULL;
    }

    if (name && findProfileByName(datastore, name)) {
        // There's already a profile with the specified name
        return NULL;
    }

    // Alocate memory for this profile
    Profile* profile = (Profile*)malloc(sizeof(Profile));
    if (!profile) {
        return NULL;
    }

    // Fill Profile data
    profile->id = id;
    profile->parentDatastore = datastore;
    profile->name = NULL;
    if (name) {
        profile->name = (char*)realloc(profile->name, strlen(name)+1);
        strcpy(profile->name, name);
    }

    if (start) {
        char* hours = strtok(start, ":");
        char* minutes = strtok(NULL, ":");

        profile->start.tm_hour = strtol(hours, (char **)NULL, 10);
        profile->start.tm_min = strtol(minutes, (char **)NULL, 10);
    }
    if (end) {
        char* hours = strtok(end, ":");
        char* minutes = strtok(NULL, ":");

        profile->end.tm_hour = strtol(hours, (char **)NULL, 10);
        profile->end.tm_min = strtol(minutes, (char **)NULL, 10);
    }

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

    if (profile->name) {
        free(profile->name);
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

Profile* findProfileByName (Datastore* datastore, const char* name) {
    if (!datastore || !name) {
        return NULL;
    }

    LL_iterator(datastore->profiles, profile_elem) {
        Profile* profile = (Profile*)profile_elem->ptr;
        if (!profile->name) {
            continue;
        }
        if (!strcmp(profile->name, name)) {
            return profile;
        }
    }

    return NULL;
}

bool isProfileActive (Profile* profile) {
    if (!profile) {
        return false;
    }

    time_t rawtime;
    struct tm *currentTime;
    time( &rawtime );
    currentTime = localtime( &rawtime );

    int startTimeInMinutes = ((profile->start).tm_hour)*60 + (profile->start).tm_min,
        endTimeInMinutes = ((profile->end).tm_hour)*60 + (profile->end).tm_min,
        currentTimeInMinutes = (currentTime->tm_hour)*60 + (currentTime->tm_min),
        timeWindow = 0;

    if (profile->end.tm_hour > profile->start.tm_hour) {
        timeWindow = endTimeInMinutes - startTimeInMinutes;
    }
    else {
        timeWindow = (24*60) - startTimeInMinutes + endTimeInMinutes;
    }

    if ((currentTimeInMinutes > startTimeInMinutes) &&
        ((currentTimeInMinutes-startTimeInMinutes) < timeWindow)) {
        
        return true;
    }
    else if (((24*60) - startTimeInMinutes + currentTimeInMinutes) < timeWindow) {
        return true;
    }

    return false;
}

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_profile = {
    NULL,
    "create_table_profile",
    "CREATE TABLE IF NOT EXISTS sinf.profile("
    "profile_id INTEGER NOT NULL PRIMARY KEY,"
    "name CHAR(30) UNIQUE,"
    "start_date TIME,"
    "end_date TIME);",
    0
};

DBQuery create_profile = {
    NULL,
    "create_profile",
    "INSERT INTO sinf.profile(profile_id, start_date, end_date) "
    "VALUES($1, to_timestamp($2, 'HH24:MI'), to_timestamp($3, 'HH24:MI'));",
    3
};

DBQuery create_named_profile = {
    NULL,
    "create_named_profile",
    "INSERT INTO sinf.profile(profile_id, start_date, end_date, name) "
    "VALUES($1, to_timestamp($2, 'HH24:MI'), to_timestamp($3, 'HH24:MI'), $4);",
    4
};

DBQuery delete_profile = {
    NULL,
    "delete_profile",
    "DELETE FROM sinf.profile WHERE profile_id=$1;",
    1
};

DBQuery create_table_profile_rule = {
    NULL,
    "create_table_profile_rule",
    "CREATE TABLE IF NOT EXISTS sinf.profile_rule("
    "profile_id INTEGER NOT NULL,"
    "rule_id INTEGER NOT NULL,"
    "FOREIGN KEY (profile_id) REFERENCES sinf.profile(profile_id) ON UPDATE CASCADE ON DELETE CASCADE,"
    "FOREIGN KEY (rule_id) REFERENCES sinf.rule(rule_id) ON UPDATE CASCADE ON DELETE CASCADE,"
    "UNIQUE (profile_id, rule_id)"
    ");",
    0
};

DBQuery add_profile_to_rule = {
    NULL,
    "add_profile_to_rule",
    "INSERT INTO sinf.profile_rule(profile_id, rule_id) "
    "VALUES($1, $2);",
    2
};

DBQuery remove_profile_from_rule = {
    NULL,
    "remove_profile_from_rule",
    "DELETE FROM sinf.profile_rule WHERE profile_id=$1 AND rule_id=$2;",
    2
};

void preparePriorityProfileQueries (list* queryList) {
    addQuerytoList(&create_table_profile, queryList);
    addQuerytoList(&create_table_profile_rule, queryList);
}

void prepareProfileQueries (list* queryList) {
    addQuerytoList(&create_profile, queryList);
    addQuerytoList(&create_named_profile, queryList);
    addQuerytoList(&delete_profile, queryList);
    addQuerytoList(&add_profile_to_rule, queryList);
    addQuerytoList(&remove_profile_from_rule, queryList);
}
