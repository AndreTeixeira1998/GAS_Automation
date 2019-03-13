#include "stdlib.h"
#include "stdio.h"

#include "DataStructure.h"
#include "ImportConfiguration.h"
#include "functions.h"

#define THREAD_READINPUT    0
#define THREAD_EXECUTERULES 1
#define THREAD_WRITEOUTPUT 2

void* readInput (void* arg) {
    Datastore* datastore = arg;
    int* ret = calloc(1, sizeof(int));
    
    // Do stuff and set the *ret return value accordingly

    pthread_exit(ret);
}

void* executeRules (void* arg) {
    Datastore* datastore = arg;
    int* ret = calloc(1, sizeof(int));
    
    // Do stuff and set the *ret return value accordingly

    pthread_exit(ret);
}

void* writeOutput (void* arg) {
    Datastore* datastore = arg;
    int* ret = calloc(1, sizeof(int));
    
    // Do stuff and set the *ret return value accordingly

    pthread_exit(ret);
}

int main(int argc, char const *argv[])
{
    Datastore* datastore = importConfiguration("GASconfig.json");
    if (!datastore) {
        printf("Error in config file.\n");
        return 1;
    }
    
    pthread_t threads[3];
    int thread_IDs[3];
    void* thread_retValues[3];
    
    // Create the threads
    thread_IDs[THREAD_READINPUT] = pthread_create(&threads[THREAD_READINPUT], NULL, &readInput, datastore);
    thread_IDs[THREAD_EXECUTERULES] = pthread_create(&threads[THREAD_EXECUTERULES], NULL, &executeRules, datastore);
    thread_IDs[THREAD_WRITEOUTPUT] = pthread_create(&threads[THREAD_WRITEOUTPUT], NULL, &writeOutput, datastore);

    // Wait for thread endings
    pthread_join(threads[THREAD_READINPUT], &thread_retValues[THREAD_READINPUT]);
    pthread_join(threads[THREAD_EXECUTERULES], &thread_retValues[THREAD_EXECUTERULES]);
    pthread_join(threads[THREAD_WRITEOUTPUT], &thread_retValues[THREAD_WRITEOUTPUT]);

    // Print Thread return values
    printf("%d\n", *(int*)thread_retValues[THREAD_READINPUT]);
    printf("%d\n", *(int*)thread_retValues[THREAD_EXECUTERULES]);
    printf("%d\n", *(int*)thread_retValues[THREAD_WRITEOUTPUT]);

    // Free memory used by threads to return a value to the parent process
    uint8_t nThreads = sizeof(thread_retValues)/sizeof(thread_retValues[0]);
    for (int i = 0; i < nThreads; i++) {
        free(thread_retValues[i]);
    }

    // Free DataStructure data
    deleteDatastore(datastore);
    
    printf("Done\n");
    
    return 0;
}
