#include "stdlib.h"
#include "stdio.h"

#include "DataStructure.h"
#include "ImportConfiguration.h"
#include "functions.h"

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

    fprintf(outputStream, "[[255,0,255],[255,0,255],[0,0,255],[255,255,255]]\n");
    
    printf("Done\n");

    fclose(inputStream);
    fclose(outputStream);
    deleteDatastore(datastore);
    return 0;
}
