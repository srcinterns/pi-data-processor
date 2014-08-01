#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "alsa.h"
#include "network.h"

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
	net_init(argv[2],argv[3]);


	for(;;) {
		// read alsa data
		// process data
		// send data

	}

	deinit_alsa();
	net_close();

	return 0;
}
