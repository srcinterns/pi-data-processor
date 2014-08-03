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
    init_processing();
	net_init(argv[2],argv[3]);


	for(;;) {

		// read alsa data
		//process_radar_data(char* intensity_time, float* trigger,
        //                 float* response, DATA_BUFFER_SIZE);
		// send data

	}

	deinit_alsa();
	net_close();
    clean_up_processing();

	return 0;
}
