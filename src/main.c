#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "alsa.h"
#include "network.h"
#include "rti.h"
#include "radar_config.h"
#include "utility.h"

#define POWER_CUTOFF (0)

char* send_data;
uint16_t* temp_buffer;
float *trigger;
float *response;

void sigint(int x)
{
    fprintf(stderr,"Received SIGINT, shutting down...\n");
    net_close();
    clean_up_processing();
    deinit_alsa();
    free(send_data);
    free(temp_buffer);
    free(trigger);
    free(response);
    exit(2);
}


int main(int argc, char ** argv)
{
/*
    if (argc != 4) {
        fprintf(stderr,"Usage: %s <device-name> <ip address> <port>\n",argv[0]);
        return 1;
    }
*/
    signal(SIGINT, sigint);
    //    init_alsa_device(argv[1]);
    //net_init(argv[2],argv[3]);
    init_processing(); //will populate size of send_array in config_array.h

    send_data = (char*) calloc(NUM_TRIGGERS*size_of_sendarray, sizeof(char));
    temp_buffer = (uint16_t*)calloc(2*DATA_BUFFER_SIZE, sizeof(signed short));
    dump_buffer = (char*)calloc(2*DATA_BUFFER_SIZE, sizeof(signed short));
    trigger = (float*) calloc(DATA_BUFFER_SIZE, sizeof(float));
    response = (float*) calloc(DATA_BUFFER_SIZE, sizeof(float));
    int i, j, count;

    count = 0;

    for(;;) {
      /* read from the audio device and store in 2 float arrays*/
      init_alsa_device(argv[1]);
        read_alsa_data((char*)temp_buffer, DATA_BUFFER_SIZE);
        s16_to_float_array(trigger, DATA_BUFFER_SIZE, 0, 1, temp_buffer);
        s16_to_float_array(response, DATA_BUFFER_SIZE, 1, 1, temp_buffer);

	for ( i = 0; i <4*DATA_BUFFER_SIZE; i++)
	  printf("%X\n", temp_buffer[i]);

        /* send_data[0][0] converts 2-d array to char*, pointing to first value*/
        process_radar_data(send_data, trigger,response, DATA_BUFFER_SIZE);

	/* send small carrier messages per row, if greater than a threshold, 
           send the index that indicates an object is there              */
        for (i = 0; i < NUM_TRIGGERS; i = i + 1){
	  printf("%d: ", count);
	  for (j = 0; j < size_of_sendarray; j++){
	    if (send_data[NUM_TRIGGERS*i +j] >= POWER_CUTOFF) {
	      // printf("%d ", send_data[NUM_TRIGGERS*i +j]);
	      //	      net_send_data(count,j);
	    }
	  }
	  printf("\n");
	  count++;
        }
	deinit_alsa();
    }


    free(send_data);
    free(temp_buffer);
    free(trigger);
    free(response);
    deinit_alsa();
    net_close();
    clean_up_processing();

    return 0;
}
