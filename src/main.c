#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "alsa.h"
#include "network.h"
#include "rti.h"
#include "radar_config.h"

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

    char send_data[NUM_TRIGGERS][size_of_sendarray];




	for(;;) {

		// read alsa data
		//process_radar_data(send_data, float* trigger,
        //                 float* response, DATA_BUFFER_SIZE);
		// send data

	}

	deinit_alsa();
	net_close();
    clean_up_processing();

	return 0;
}
