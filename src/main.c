#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "alsa.h"
#include "network.h"
#include "rti.h"
#include "radar_config.h"
#include "utility.h"

#define POWER_CUTOFF (50)

char* send_data;
int16_t* temp_buffer;
float *trigger;
float *response;
FILE * pcmfile = NULL;
char alsa_device[255] = {0};
char target_ip[16] = {0};
char target_port[6] = {0};

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

void print_help(void)
{
	printf("Usage: ./processor  [-a|-f] <source> [-i] <dest IP> [-p] <dest Port>\n");
	printf("\t-a\tUse alsa device defined by <source>\n");
	printf("\t-f\tRead in from raw pcm data in <source>\n");
	printf("\t-i\tTarget IP to send data to\n");
	printf("\t-p\tTarget Port\n");

}

void handle_args(int argc, char ** argv)
{
	int i, mode = 0;
	for (i = 1; i < argc; i++) {
		switch(argv[i][1]) {
			case 'a': strcpy(alsa_device, argv[++i]);  mode++; break;
			case 'f': pcmfile = fopen(argv[++i], "r"); mode--; break;
			case 'i': strcpy(target_ip, argv[++i]);            break;
			case 'p': strcpy(target_port, argv[++i]);          break;
			default:  print_help(), exit(1);                   break;
		}
	}
	if (0 == mode) {
		fprintf(stderr,"Error: Either -a or -f need to be specified\n");
		exit(1);
	} else if ('\0' == target_ip[0]) {
		fprintf(stderr,"Error: Improper (or no) IP Address specified\n");
		exit(1);
	} else if ('\0' == target_port[0]) {
		fprintf(stderr,"Error: Improper (or no) Port specified\n");
		exit(1);
	} else if (NULL == pcmfile) {
		fprintf(stderr,"Error opening file!\n");
		exit(1);
	}
	printf("Sending to %s:%s\n", target_ip, target_port);
}

int main(int argc, char ** argv)
{

    if (argc < 2) {
		print_help();
        return 1;
    }
	handle_args(argc,argv);

    signal(SIGINT, sigint);
    //    init_alsa_device(alsa_device);
    net_init(target_ip,target_port);
    init_processing(); //will populate size of send_array in config_array.h

    send_data = (char*) calloc(NUM_TRIGGERS*size_of_sendarray, sizeof(char));
    temp_buffer = (int16_t*)calloc(2*DATA_BUFFER_SIZE, sizeof(signed short));
    dump_buffer = (char*)calloc(2*DATA_BUFFER_SIZE, sizeof(signed short));
    trigger = (float*) calloc(DATA_BUFFER_SIZE, sizeof(float));
    response = (float*) calloc(DATA_BUFFER_SIZE, sizeof(float));
    int i, j, count;

    count = 0;

    for(;;) {
      /* read from the audio device and store in 2 float arrays*/
      //init_alsa_device(argv[1]);


      /* if the pcmfile is define then read from the file, else read the
       * alse data                                                     */
      if (NULL != pcmfile) {
	fprintf(stderr,"read from file\n");
	if (feof(pcmfile)) {
	  fprintf(stderr,"End of file reached!\n");
	  break;
	}
	fread(temp_buffer, sizeof(int16_t), DATA_BUFFER_SIZE * 2, pcmfile); 
			
      } else {
	read_alsa_data((char*)temp_buffer, DATA_BUFFER_SIZE);
	printf("read from alsa");
      }



      s16_to_float_array(trigger, DATA_BUFFER_SIZE, 1, 1, temp_buffer);
      s16_to_float_array(response, DATA_BUFFER_SIZE, 0, 1, temp_buffer);

      /* send_data[0][0] converts 2-d array to char*, pointing to first value*/
      process_radar_data(send_data, trigger, response, DATA_BUFFER_SIZE);

      /* send small carrier messages per row, if greater than a threshold, 
	 send the index that indicates an object is there              */
      for (i = 0; i < NUM_TRIGGERS; i = i + 1){
	printf("%d: ", count);
	for (j = 0; j < size_of_sendarray; j++){
	  if (send_data[NUM_TRIGGERS*i +j] >= POWER_CUTOFF) {
	    printf("%d ", j);
	    //net_send_data(count,j);
	  }
	}
	printf("\n");
	count++;
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
