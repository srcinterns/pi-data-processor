#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "alsa.h"
#include "network.h"
#include "rti.h"
#include "radar_config.h"
#include "messagize.h"


#define MESSAGE_ID (0x1)

void sigint(int x)
{
    fprintf(stderr,"Received SIGINT, shutting down...\n");
    net_close();
    clean_up_processing();
    deinit_alsa();
    exit(2);
}


int main(int argc, char ** argv)
{

    if (argc != 4) {
        fprintf(stderr,"Usage: %s <device-name> <ip address> <port>\n",argv[0]);
        return 1;
    }

    signal(SIGINT, sigint);
    init_alsa_device(argv[1]);
    net_init(argv[2],argv[3]);
    init_processing(); //will populate size of send_array in config_array.h

    char* send_data = (char*) calloc(NUM_TRIGGERS*size_of_sendarray, sizeof(char));
    signed short* temp_buffer = (signed short*)calloc(2*DATA_BUFFER_SIZE, sizeof(signed short));
    float *trigger = (float*) calloc(DATA_BUFFER_SIZE, sizeof(float));
    float *response = (float*) calloc(DATA_BUFFER_SIZE, sizeof(float));
    int i;

    for(;;) {

        read_alsa_data((char*)temp_buffer, DATA_BUFFER_SIZE);
        s16_to_float_array(trigger, DATA_BUFFER_SIZE, 0, 1, temp_buffer);
        s16_to_float_array(response, DATA_BUFFER_SIZE, 1, 1, temp_buffer);


        /* send_data[0][0] converts 2-d array to char*, pointing to first value*/
        process_radar_data(&send_data[0][0], trigger,
                           response, DATA_BUFFER_SIZE);
        for (i = 0; i < NUM_TRIGGERS; i = i + 8){
            messagize_and_send(&send_data[NUM_TRIGGERS*i], size_of_sendarray);
        }
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
