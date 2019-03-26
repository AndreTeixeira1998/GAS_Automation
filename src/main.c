#include "stdlib.h"
#include "stdio.h"
#include <string.h>

#include "DataStructure.h"
#include "functions.h"
#include "ImportConfiguration.h"

//Here some definitions for stings and data sizes
#define BUFFER 256 
#define MAX_DATASIZE 5 
#define PAYLOAD_SIZE 23 

//Denomination and respective positions of the arrays
#define START 0 
#define AM_MESSAGE 2
#define DESTINATION_ADDRESS 3
#define MOTE_ID 5
#define PAYLOAD_LENGHT 7
#define GROUP_ID 8
#define RAW_VOLTAGE 10
#define RAW_VISIBLE_LIGHT 12
#define RAW_CURRENT 14
#define RAW_TEMPERATURE 16
#define RAW_HUMIDITY 18
#define MESSAGE_HANDLING_INFO 20
#define END 22

// RGB Matrix configuration
#define X_SIZE  3
#define Y_SIZE  3

#define DEFAULT_COLOR_R PIXEL_DEFAULT_RED
#define DEFAULT_COLOR_G PIXEL_DEFAULT_GREEN
#define DEFAULT_COLOR_B PIXEL_DEFAULT_BLUE

// Multithreading
#define THREAD_READINPUT    0
#define THREAD_EXECUTERULES 1
#define THREAD_WRITEOUTPUT  2

typedef struct {
    Datastore* datastore;
    FILE* stream;
}ThreadArgs;

void* thread_readInput (void* arg) {
    ThreadArgs* args = arg;
    Datastore* datastore = args->datastore;
    FILE* stream = args->stream;
    int* ret = calloc(1, sizeof(int));
    
    char str[BUFFER +1];
    char *token;
    char *endptr; 
    char data[PAYLOAD_SIZE][MAX_DATASIZE];
    int data_type=0, c_vect_index, converted_data[PAYLOAD_SIZE];

    while (1) {    
        //while(fgets(str, BUFFER , fp)){
        if (fgets(str, BUFFER, stream)) {
            
            //printf("%s\n", str);
            
            token = strtok(str, " \n"); //here starts a loop to separate data into respective positions
            
            while (token != NULL) { // Loop until no character is found on the string
            
                strcpy(data[data_type], token);
                
                if(data_type==(START +1) || data_type==(DESTINATION_ADDRESS+1) || data_type==(MOTE_ID+1) || data_type==(GROUP_ID+1) || data_type==(RAW_VOLTAGE+1) || data_type==(RAW_VISIBLE_LIGHT+1) || data_type==(RAW_CURRENT+1) || data_type==(RAW_TEMPERATURE+1) || data_type==(RAW_HUMIDITY+1) || data_type==(MESSAGE_HANDLING_INFO+1)) {
                    strcat(data[data_type-1], data[data_type]); //if the attribute needs 4 hex digits we concatenate the next value into the correct position
                }
                
                data_type++;

                token = strtok(NULL, " \n");
                
                if (data_type == PAYLOAD_SIZE){
                    // Restart postitions
                    data_type=0;
                }
                
            }

            for (c_vect_index = 0; c_vect_index < PAYLOAD_SIZE; c_vect_index++) {
                // Converts hex data to dec
                converted_data[c_vect_index] = strtol(data[c_vect_index], &endptr, 16);
            }
            
            // Here we set the sensor values with converted data in its respective Node IDs and Sensor Type
            setSensorValue (findSensorByType (findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_VOLTAGE), converted_data[RAW_VOLTAGE]);
            setSensorValue (findSensorByType (findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_TEMPERATURE), converted_data[RAW_TEMPERATURE]);
            setSensorValue (findSensorByType (findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_HUMIDITY), converted_data[RAW_HUMIDITY]);
            setSensorValue (findSensorByType (findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_LIGHT), converted_data[RAW_VISIBLE_LIGHT]);
            setSensorValue (findSensorByType (findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_CURRENT), converted_data[RAW_CURRENT]);
            
            
            // some printfs for debugging
            /*if(findSensorByType(findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_HUMIDITY)==NULL) 
                puts("Humidity sensor missing");
            
            printf("Sensor ID: #%d# T= %f ºC\n", converted_data[MOTE_ID], getSensorValue (findSensorByType(findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_TEMPERATURE)));
            printf("Sensor ID: #%d# I= %f A\n", converted_data[MOTE_ID], getSensorValue (findSensorByType(findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_CURRENT)));
            printf("Sensor ID: #%d# H= %f Kg/m3\n", converted_data[MOTE_ID], getSensorValue (findSensorByType(findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_HUMIDITY)));
            printf("Sensor ID: #%d# V= %f V\n", converted_data[MOTE_ID], getSensorValue (findSensorByType(findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_VOLTAGE)));
            printf("Sensor ID: #%d# L= %f lx\n", converted_data[MOTE_ID], getSensorValue (findSensorByType(findNodeByID (datastore, converted_data[MOTE_ID]), TYPE_SENSOR_LIGHT)));

            printf("----------------------------------------------------------------------\n");*/
            
        }
    }

    pthread_exit(ret);
}

void* thread_executeRules (void* arg) {
    ThreadArgs* args = arg;
    Datastore* datastore = args->datastore;
    //FILE* stream = args->stream;
    int* ret = calloc(1, sizeof(int));
    
    while (1) {
        executeRules(datastore);
    }

    pthread_exit(ret);
}

void* thread_writeOutput (void* arg) {
    ThreadArgs* args = arg;
    Datastore* datastore = args->datastore;
    FILE* stream = args->stream;
    int* ret = calloc(1, sizeof(int));
    
    while (1) {
        fprintf(stream, "[");
        for (int x = 0; x < X_SIZE; x++) {
            for (int y = 0; y < Y_SIZE; y++) {
                Position position;
                position.x = x;
                position.y = y;

                Pixel* pixel = findPixelByPos(datastore, &position);
                if (pixel) {
                    pthread_mutex_lock(&pixel->mutex);
                    fprintf(stream, "[%d,%d,%d]", pixel->color->r, pixel->color->g, pixel->color->b);
                    pthread_mutex_unlock(&pixel->mutex);
                }
                else {
                    fprintf(stream, "[%d,%d,%d]", DEFAULT_COLOR_R, DEFAULT_COLOR_G, DEFAULT_COLOR_B);
                }

                if (y < X_SIZE-1) {
                    fprintf(stream, ",");
                }
            }
            if (x < Y_SIZE-1) {
                fprintf(stream, ",");
            }
        }
        fprintf(stream, "]\n");
    }

    pthread_exit(ret);
}


int main(int argc, char const *argv[]) {
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

    pthread_t threads[3];
    int thread_IDs[3];
    void* thread_retValues[3];
    ThreadArgs thread_args[3];

    // Prepare thread arguments
    thread_args[THREAD_READINPUT].datastore = datastore;
    thread_args[THREAD_READINPUT].stream = inputStream;

    thread_args[THREAD_EXECUTERULES].datastore = datastore;
    thread_args[THREAD_EXECUTERULES].stream = NULL;

    thread_args[THREAD_WRITEOUTPUT].datastore = datastore;
    thread_args[THREAD_WRITEOUTPUT].stream = outputStream;

    
    // Create the threads
    thread_IDs[THREAD_READINPUT] = pthread_create(&threads[THREAD_READINPUT], NULL, &thread_readInput, &thread_args[THREAD_READINPUT]);
    thread_IDs[THREAD_EXECUTERULES] = pthread_create(&threads[THREAD_EXECUTERULES], NULL, &thread_executeRules, &thread_args[THREAD_EXECUTERULES]);
    thread_IDs[THREAD_WRITEOUTPUT] = pthread_create(&threads[THREAD_WRITEOUTPUT], NULL, &thread_writeOutput, &thread_args[THREAD_WRITEOUTPUT]);

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
    
    
    printf("Done\n");

    fclose(inputStream);
    fclose(outputStream);
    deleteDatastore(datastore);
    return 0;
}