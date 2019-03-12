#include "stdlib.h"
#include "stdio.h"
#include <string.h>

#include "DataStructure.h"
#include "ImportConfiguration.h"
#include "functions.h"

#define BUFFER 256 
#define MAX_DATASIZE 5 
#define PAYLOAD_SIZE 23 

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

char* filterString (char* str) {
    if (!str) {
        return NULL;
    }

    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == '\r' || str[i] == '\n' || str[i] == '#') {
            str[i] = '\0';
            return str;
        }
    }

    return str;
}


int main(int argc, char const *argv[])
{
    Datastore* datastore = importConfiguration("GASconfig.json");
    if (!datastore) {
        printf("Error in config file.\n");
        return 1;
    }

    FILE *fp;
    char filename[BUFFER + 1];
    char str[BUFFER +1];
    char *token;
    char *endptr; 
    char data[PAYLOAD_SIZE][MAX_DATASIZE];
    int data_type=0, k=0, c_vect_index, converted_data[PAYLOAD_SIZE];

    puts("chateei-me com os channels por isso por agora mandem o path para a file");
    
    fgets( filename, BUFFER, stdin);
    //gets(filename);
    filterString (filename);
    
    printf("\n\n\nOpening #%s#\n\n", filename );
    
    fp=fopen(filename, "r");
    
    if(fp){
        
        printf("\n\n\nOpening #%s#\n\n", filename );
        
        while(fgets(str, BUFFER , fp)){
            printf("%s\n", str);
            token= strtok(str, " \n");
            while(token !=NULL){
                //printf("token:%s\n",token); // debugging

                //printf("#%s# - %d\n", data[data_type], data_type); //debugging
                
                strcpy(data[data_type], token);
                //printf("#%s# - %d\n", data[data_type], data_type); //debugging
                if(data_type==(START +1) || data_type==(DESTINATION_ADDRESS+1) || data_type==(MOTE_ID+1) || data_type==(GROUP_ID+1) || data_type==(RAW_VOLTAGE+1) || data_type==(RAW_VISIBLE_LIGHT+1) || data_type==(RAW_CURRENT+1) || data_type==(RAW_TEMPERATURE+1) || data_type==(RAW_HUMIDITY+1) || data_type==(MESSAGE_HANDLING_INFO+1)) {
                    strcat(data[data_type-1], data[data_type]);
                }
                //printf("#%s# - %d\n", data[data_type], data_type); //debugging

                data_type++;
                token= strtok(NULL, " \n");
                
                if(data_type==PAYLOAD_SIZE) data_type=0;
                //printf("#%s# - %d\n", data[data_type], data_type); //debugging

            }

            //printf("#%s# - %d\n", data[MOTE_ID], MOTE_ID); //debugging
            
            for (c_vect_index=0; c_vect_index<PAYLOAD_SIZE; c_vect_index++){ 
                //printf("#%s# - %d\n", data[c_vect_index], c_vect_index);  //DEBUGGING
                converted_data[c_vect_index]= strtol(data[c_vect_index], &endptr, 16);
                printf("hex: %s | dec: %d - vect_index: (%d)\n", data[c_vect_index], converted_data[c_vect_index], c_vect_index);  //DEBUGGING
 
            }
            
            //for (k=0; k<23; k++){ printf("#%s# - %d\n", data[k], k); }//debugging
            
            // aqui falta meter um switch para meter os valores
            
            printf("----------------------------------------------------------------------\n");
	    }

        fclose(fp);

    } else {

        puts("error opening file to read"); 
    
    }
    	
    printf("\n\nDone\n");
    return 0;
}
