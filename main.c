#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "alsa.h"

int main(int argc, char ** argv)
{

	char buffer[1024];
	FILE * output;
	int size;
	int i;
	time_t now, end;

	if (argc != 3) {
		fprintf(stderr,"Usage: %s <device-name> <output-file>\n",argv[0]);
		return 1;
	}
	output = fopen(argv[2],"w");

	init_alsa_device(argv[1]);

	time(&end);
	end += 10;
	for (; now != end ; time(&now)) {
		size = read_alsa_data(buffer);
		if (-1 == size) break;

		printf("\rRecording for %ld more seconds...                           ",end-now);

		fwrite(buffer, 1, size, output);
	}
	printf("\n");
	fclose(output);

	deinit_alsa();

	return 0;
}
