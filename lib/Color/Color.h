#ifndef __COLOR__
#define __COLOR__

#include <stdint.h>
#include <stdlib.h>

#include "LinkedList.h"

typedef struct _color Color;

/**
 * @brief Structure to hold a color in RGB format
 * 
 */
struct _color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

#endif
