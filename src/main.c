#include "stdlib.h"
#include "stdio.h"

#include "DataStructure.h"
#include "ImportConfiguration.h"
#include "functions.h"

#define X_SIZE  3
#define Y_SIZE  3

#define DEFAULT_COLOR_R 0
#define DEFAULT_COLOR_G 0
#define DEFAULT_COLOR_B 0

int main(int argc, char const *argv[])
{
    if (argc < 4) {
        printf("Not enough arguments. Expecting:\n\t%s <configuration-file> <input-stream> <output-stream>\n\n", argv[0]);
        return 1;
    }

    Datastore* datastore = importConfiguration(argv[1]);
    if (!datastore) {
        printf("Error in config file.\n");
        return 1;
    }
    
    
    FILE* inputStream = fopen(argv[2], "r");
    FILE* outputStream = fopen(argv[3], "w");
    if (!inputStream || !outputStream) {
        printf("Error reading streams. Please verify.\n");
        return 1;
    }

    
    fprintf(outputStream, "[");
    for (int x = 0; x < X_SIZE; x++) {
        for (int y = 0; y < Y_SIZE; y++) {
            Position position;
            position.x = x;
            position.y = y;

            Pixel* pixel = findPixelByPos(datastore, &position);
            if (pixel) {
                fprintf(outputStream, "[%d,%d,%d]", pixel->color->r, pixel->color->g, pixel->color->b);
            }
            else {
                fprintf(outputStream, "[%d,%d,%d]", DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B);
            }

            if (y < X_SIZE-1) {
                fprintf(outputStream, ",");
            }
        }

        if (x < Y_SIZE-1) {
            fprintf(outputStream, ",");
        }
    }
    fprintf(outputStream, "]\n");
    
    printf("Done\n");

    fclose(inputStream);
    fclose(outputStream);
    deleteDatastore(datastore);
    return 0;
}
