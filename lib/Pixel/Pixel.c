#include "Pixel.h"

Pixel* createPixel (Datastore* datastore, Color* color, Position* pos) {
    if (!datastore || !pos) {
        return NULL;
    }

    if (findPixelByPos(datastore, pos)) {
        return NULL;
    }

    Color* pixelColor = (Color*)malloc(sizeof(Color));
    if (pixelColor == NULL) {
        // Memory allocation failed
        return NULL;
    }

    if (color) {
        pixelColor->r = color->r;
        pixelColor->g = color->g;
        pixelColor->b = color->b;
    }
    else {
        pixelColor->r = PIXEL_DEFAULT_RED;
        pixelColor->g = PIXEL_DEFAULT_GREEN;
        pixelColor->b = PIXEL_DEFAULT_BLUE;
    }


    Position* pixelPos = (Position*)malloc(sizeof(Position));
    if (pixelPos == NULL) {
        // Memory allocation failed
        free(pixelColor);
        return NULL;
    }

    pixelPos->x = pos->x;
    pixelPos->y = pos->y;


    Pixel* pixel = (Pixel*)malloc(sizeof(Pixel));
    if (pixel == NULL) {
        free(pixelColor);
        free(pixelPos);
        // Memory allocation failed
        return NULL;
    }
    
    if(pthread_mutex_init(&pixel->mutex, NULL)) {
        // Mutex init failed
        free(pixelColor);
        free(pixelPos);
        return NULL;
    }

    list_element* elem = listInsert(datastore->pixels, pixel, NULL);
    if (elem == NULL) {
        // Insertion failed
        pthread_mutex_destroy(&pixel->mutex);
        free(pixelColor);
        free(pixelPos);
        free(pixel);
        return NULL;
    }

    pixel->remote_id = 0;
    pixel->color = pixelColor;
    pixel->pos = pixelPos;
    pixel->listPtr = elem;
    pixel->parentDatastore = datastore;

    return pixel;
}

bool deletePixel (Pixel* pixel) {
    if (!pixel) {
        return true;
    }

    list_element* elem = pixel->listPtr;
    Datastore* datastore = pixel->parentDatastore;

    pthread_mutex_destroy(&pixel->mutex);
    free(pixel->color);
    free(pixel->pos);
    free(pixel);

    list_element* res = listRemove(datastore->pixels, elem);
    if (res == NULL && listSize(datastore->pixels)) {
        return true;
    }

    return false;
}

bool setPixelColor (Pixel* pixel, Color* color) {
    if (!pixel || !color) {
        return true;
    }

    pthread_mutex_lock(&pixel->mutex);
    pixel->color->r = color->r;
    pixel->color->g = color->g;
    pixel->color->b = color->b;
    pthread_mutex_unlock(&pixel->mutex);

    return false;
}

Color* getPixelColor (Pixel* pixel) {
    if (!pixel) {
        return NULL;
    }

    return pixel->color;
}

bool setPixelPosition (Pixel* pixel, Position* pos) {
    if (!pixel || !pos) {
        return true;
    }

    pixel->pos->x = pos->x;
    pixel->pos->y = pos->y;

    return false;
}

Position* getPixelPosition (Pixel* pixel) {
    if (!pixel) {
        return NULL;
    }

    return pixel->pos;
}

Pixel* findPixelByPos (Datastore* datastore, Position* pos) {
    if (!datastore || !pos) {
        return NULL;
    }

    LL_iterator(datastore->pixels, pixel_elem) {
        Pixel* pixel = pixel_elem->ptr;
        if (pixel->pos->x == pos->x && pixel->pos->y == pos->y) {
            return pixel;
        }
    }

    return NULL;
}

/**********************************/
/*        DATABASE QUERIES        */
/**********************************/

DBQuery create_table_pixel = {
    NULL,
    "create_table_pixel",
    "CREATE TABLE IF NOT EXISTS sinf.pixel("
    "pixel_id SERIAL UNIQUE NOT NULL,"
    "x_position INTEGER NOT NULL,"
    "y_position INTEGER NOT NULL,"
    "PRIMARY KEY (x_position, y_position)"
    ");",
    0
};

DBQuery create_pixel = {
    NULL,
    "create_pixel",
    "INSERT INTO sinf.pixel(x_position, y_position) "
    "VALUES($1, $2) "
    "RETURNING pixel_id;",
    2
};

DBQuery delete_pixel = {
    NULL,
    "delete_pixel",
    "DELETE FROM sinf.pixel WHERE pixel_id=$1;",
    1
};

void preparePriorityPixelQueries (list* queryList) {
    addQuerytoList(&create_table_pixel, queryList);
}

void preparePixelQueries (list* queryList) {
    addQuerytoList(&create_pixel, queryList);
    addQuerytoList(&delete_pixel, queryList);
}