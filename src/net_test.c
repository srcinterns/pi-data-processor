#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "network.h"

static void sigint(int x)
{
	net_close();
	exit(1);
}


int main(int argc, char ** argv)
{
	uint32_t segment;
	uint16_t index;
	int i;

	signal(SIGINT, sigint);

	if (3 != argc) {
		fprintf(stderr, "Usage: %s <ip> <port>\n", argv[0]);
		return 1;
	}

	net_init(argv[1],argv[2]);



	for (;;) {
		for (i = 0; i < segment; (index = i++ % 3) && net_send_data(segment, index));
		fprintf(stderr,"\rpushed segment %d",segment++);
		sleep(1);
	}
	net_close();

	return 0;
}
