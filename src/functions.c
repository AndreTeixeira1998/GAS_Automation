#include "functions.h"
#include "stdio.h"

/*void skipComments (FILE* file, char* line) {
    
    do {
        line = fgets(line, CONFIGURATION_FILE_MAX_LINE_SIZE, file);
    } while (!strncmp(line, "#", 1));

    return;
}*/

char* filterString (char* str) {
    if (!str) {
        return NULL;
    }

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '\r' || str[i] == '\n' || str[i] == '#') {
            str[i] = '\0';
            return str;
        }
    }

    return str;
}

char* getMinifiedJSONStringFromFile (const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    int readChar = 0;
    unsigned int jsonSize = 0,
        jsonReserved = 100;
    bool inString = false;
    char* json = malloc(jsonReserved);
    if (!json) {
        fclose(file);
        return NULL;
    }
    json[0] = '\0';

    while ( (readChar = fgetc(file)) != EOF) {
        // Ignore line endings and spaces (if not in string)
        if ( (readChar == ' ' && !inString)
            || readChar == '\n'
            || readChar == '\r') {
            continue;
        }

        // Check if string is being read
        if (readChar == '"' || readChar == '\'') {
            inString = !inString;
        }

        // Check if there is still space in the allocated string
        if (jsonSize == jsonReserved-1) {
            jsonReserved += 100;
            json = realloc(json, jsonReserved);
            if (!json) {
                fclose(file);
                return NULL;
            }
        }

        // Concat current JSON string and new char
        char aux[2] = {readChar, '\0'};
        strcat(json, aux);
        jsonSize++;
    }

    // Trim string to actual size of JSON's minified string
    json = realloc(json, jsonSize+1);

    // Close the file
    fclose(file);

    return json;
}

Datastore* importConfiguration(const char* filename) {
    
    char* json = getMinifiedJSONStringFromFile(filename);
    //char* json = "{\"rooms\":[{\"id\":0,\"name\":\"My Bedroom\",\"nodes\":[{\"id\":0,\"sensors\":[{\"type\":0},{\"type\":1}],\"actuators\":[{\"posX\":10,\"posY\":1}]}]}]}";

    jsmn_parser parser;
    jsmn_init(&parser);
    int tokensRequired = jsmn_parse(&parser, json, strlen(json), NULL, 0);
    jsmntok_t tokens[tokensRequired];

    int res = jsmn_parse(&parser, json, strlen(json), tokens, sizeof(tokens)/sizeof(tokens[0]));
    printf("req: %d | res: %d\n", tokensRequired, res);
    if (res < 0) {
        free(json);
        printf("cenas\n");
        return NULL;
    }

    printf("%s\n", json);
    free(json);
    return NULL;
}