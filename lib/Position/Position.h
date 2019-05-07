#ifndef __POSITION__
#define __POSITION__

#include <stdint.h>
#include <stdlib.h>

typedef struct _position Position;

/**
 * @brief Structure to hold a cartesian position
 * 
 */
struct _position {
    uint16_t x;
    uint16_t y;
};

#endif
