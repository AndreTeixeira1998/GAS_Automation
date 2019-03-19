#include "stdlib.h"
#include "stdio.h"

#include "DataStructure.h"
#include "ImportConfiguration.h"
#include "functions.h"

int main(int argc, char const *argv[])
{
    Datastore* datastore = importConfiguration("GASconfig.json");
    if (!datastore) {
        printf("Error in config file.\n");
        return 1;
    }
    
    if (argc < 3) {
        printf("Not enough arguments. Expecting:\n\t%s <input-stream> <output-stream>\n\n", argv[0]);
        return 1;
    }

    FILE* inputStream = fopen(argv[1], "r");
    FILE* outputStream = fopen(argv[2], "w");
    if (!inputStream || !outputStream) {
        printf("Error reading streams. Please verify.\n");
        return 1;
    }
    
    printf("Done\n");

    fclose(inputStream);
    fclose(outputStream);
    return 0;
}
