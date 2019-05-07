#ifndef __PIXEL__
#define __PIXEL__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "LinkedList.h"

#include "Datastore.h"
#include "Position.h"
#include "Color.h"

#define PIXEL_DEFAULT_RED       255
#define PIXEL_DEFAULT_GREEN     255
#define PIXEL_DEFAULT_BLUE      255

typedef struct _pixel Pixel;

/**
 * @brief Structure to hold all data regarding a pixel in the RGB Matrix
 * 
 */
struct _pixel {
    Position* pos;
    Color* color;
    list_element* listPtr;
    Datastore* parentDatastore;
    pthread_mutex_t mutex;
};

/**
 * @brief Create a Pixel object
 * 
 * @param datastore Datastore to save the Pixel
 * @param color Color of the pixel
 * @param pos Position of the Pixel
 * @return Pixel* Pointer to the new Pixel object. NULL if error.
 */
Pixel* createPixel (Datastore* datastore, Color* color, Position* pos);

/**
 * @brief Delete a Pixel object
 * 
 * @param actuator The pointer to the Pixel object to be deleted.
 * @return true Error
 * @return false All good
 */
bool deletePixel (Pixel* pixel);

/**
 * @brief Set the Pixel Color object
 * 
 * @param pixel Pointer to the Pixel object
 * @param color Pointer to a Color object with the desired specs
 * @return true Error
 * @return false All good
 */
bool setPixelColor (Pixel* pixel, Color* color);

/**
 * @brief Get the Pixel Color object
 * 
 * @param actuator Pointer to the Pixel object
 * @return Color* Pointer to the Color of the Pixel. NULL if error.
 */
Color* getPixelColor (Pixel* pixel);

/**
 * @brief Set the Pixel Position object
 * 
 * @param pixel Pointer to the Pixel object
 * @param pos Pointer to a Position object with the desired specs
 * @return true Error
 * @return false All good
 */
bool setPixelPosition (Pixel* pixel, Position* pos);

/**
 * @brief Get the Pixel Position object
 * 
 * @param pixel Pointer to the Pixel object
 * @return Position* Pointer to the Position of the Pixel. NULL if error.
 */
Position* getPixelPosition (Pixel* pixel);

/**
 * @brief Searches the datastore for a Pixel with the specified position
 * 
 * @param datastore Datastore object to search
 * @param pos Position on the RGB Matrix output
 * @return Pixel* Pixel object with the desired position. NULL if not found.
 */
Pixel* findPixelByPos (Datastore* datastore, Position* pos);

#endif