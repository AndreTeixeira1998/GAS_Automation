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
    
    
    
    printf("Done\n");
    
    return 0;
}
