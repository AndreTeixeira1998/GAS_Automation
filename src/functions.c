#include "functions.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

char* filterLineEndings (char* str) {
    if (!str) {
        return NULL;
    }

    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == '\r' || str[i] == '\n' || str[i] == '#') {
            str[i] = '\0';
            return str;
        }
    }

    return str;
}
