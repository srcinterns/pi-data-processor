/*******************************************************/
/* RTI.c - David Campbell
 *  8/2/2014
 *  personal email: dncswim76@gmail.com
 *  src email dcampbell@srcinc.com
 *
 *  This file contains the API for processing radar data
 *  that comes in from the audio device.  It is taylored
 *  to work with the parameters that are laid out in
 *  radar_config.h.
 *
 *  In general, there is data on two channels of
 *  information.  The "trigger" channel contains
 *  information that allows us to frame data into
 *  a 2D array such that each element contains
 *  the data within that pulses from the "response"
 *  channel.  Once each array has been made for
 *  each pulse, processing can be done on each
 *  pulse to remove noise, translate incoming
 *  frequencies to distance (via an ifft) and
 *  map the frequencies to the power of the signal
 *  (using the dbv function).  The final goal
 *  will be to translate the data into a scale
 *  of 0 to 255 based off the intensity of the
 *  signal after it is processed*/
/********************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "radar_config.h"
#include "rti.h"
#include "dbv.h"
#include "ifft_wrapper.h"
#include <assert.h>
#include <math.h>
#include <string.h>

//#define PRINT_TRIGGERING
//#define PRINT_PARSED
//#define PRINT_IFFT
//#define PRINT_AVERAGED
//#define PRINT_CANCELOR
#define PRINT_FINAL

static char* start;
static rdata_t* ifft_array;


/* FIND_TRIGGER_START - given the trigger array, if the data is greater
 * than a certain threshold, mark in the "start" array that value is
 * greater than the threshold.  Both arrays are meant to be of array size.*/
void find_trigger_start(rdata_t* trigg_array, char* start, int array_size){
   int i;

   assert(trigg_array != NULL);
   assert(start != NULL);

   for (i = 0; i < array_size; i++) {

     if (trigg_array[i] > THRESHOLD){
	start[i] = 1;
     }else
	start[i] = 0;

   }

   return;
}

int none(char* array, int start, int stop){

   int i;

   assert(array != NULL);

   for (i = start; i <= stop; i++){
     if (array[i] != 0)
       return FALSE;
   }
   return TRUE;

}

/* MEAN - find the average of the array over
*   the specified interval.  Both start and
*   stop are inclusive                    */
rdata_t mean(rdata_t* array, int start, int stop){

   int i;
   rdata_t sum = 0;
   rdata_t count = 0;

   assert(array != NULL);

   for (i = start; i <= stop; i++){
     sum = sum + array[i];
     count++;
   }
   return sum/count;

}

/*ABS - find the absolute value of a rdata_t*/
rdata_t abs_value(rdata_t value){
  if (value < 0.0)
    return -1.0*value;
  return value;
}

/*RDATA_T COPY - copy the elements from one rdata_t array
 * to another specified by a certain amount*/
void rdata_t_cpy(rdata_t* arrayA, rdata_t* arrayB, int amount){
  int i;

  assert(arrayA != NULL);
  assert(arrayB != NULL);

  for (i = 0; i < amount; i++){
    arrayA[i] = arrayB[i];
  }
}


/*RDATA_T ADD IMMEDIATE - add a scalar value
 * from all the elements in the rdata_t array*/
void rdata_t_addi(rdata_t* arrayA, rdata_t value, int size){
  int i;

  assert(arrayA != NULL);

  for (i = 0; i < size; i++){
    arrayA[i] = arrayA[i] + value;
  }
}

/*SUBTRACT ARRAY - arrayA[i] = arrayB[i] - arrayA[i]*/
void sub_array(rdata_t* arrayA, rdata_t* arrayB, int size){
    int i;

    assert(arrayA != NULL);
    assert(arrayB != NULL);

    for (i = 0; i < size; i++)
        arrayA[i] = arrayB[i] - arrayA[i];
}

/*INTENSIFY - make data into a value from 0 to 255*/
void intensify(char* char_array, rdata_t* rdata_t_array, int size){
  int i;
  rdata_t temp_rdata_t;
  int temp;

  assert(char_array != NULL);
  assert(rdata_t_array != NULL);

  for (i = 0; i < size; i++){
    temp_rdata_t  = rdata_t_array[i] + 80;

    if (temp_rdata_t < 0)
        temp_rdata_t = 0;

    temp = floorf(((temp_rdata_t/80.0)*255));

    if (temp > 255)
      temp = 255;

    char_array[i] = (char) temp;
  }
}


/*FIND MAX - find that maximum value in the
 * array*/
rdata_t find_max(rdata_t* array, int size){
    int i;
    rdata_t max = 0;

    assert(array != NULL);

    for ( i = 0; i < size; i++){
        if (array[i] > max)
            max = array[i];
    }

    return max;
}


/*INIT PROCESSING -Set up the environment for the signal processing*/
void init_processing(){
    size_of_sendarray = init_fft(SAMPLES_PER_PULSE);
    start = calloc(DATA_BUFFER_SIZE, sizeof(char));
    ifft_array = calloc(size_of_sendarray, sizeof(rdata_t));

}

/*CLEAN UP PROCESSING - clean up the environment for signal processing*/
void clean_up_processing(){
    end_fft();
    free(start);
    free(ifft_array);
}

/*PARSE RADAR DATA - traverse the two data array's and
 * use the start_array to determine
 * the start of a pulse.  Keep track of the pulse,
 * and log the pulse data based on the
 * pulse count into a 2 dimensional array, response_parsed.
 * NOTE: intensity_time must be of size NUM_TRIGGERS*SAMPLES_PER_PULSE*/
void process_radar_data(char* intensity_time,
                      rdata_t* trigger, rdata_t* response, int buf_size){

   int count = 0;
   int i, j;
   rdata_t average, max;

   assert(intensity_time != NULL);
   assert(trigger != NULL);
   assert(response != NULL);

   /*for size of radar data to be correct, init_processing must be
    *called before this function                                */
   rdata_t response_parsed[NUM_TRIGGERS][size_of_sendarray];

   /*create a simplied edge trigger based off the transmit signal*/
   memset(start, 0, buf_size);
   find_trigger_start(trigger, start, buf_size);

#ifdef PRINT_TRIGGERING
   for (i = 0; i < buf_size; i++) {
     printf("%d: %d %f %f\n", i, start[i], trigger[i], response[i]);
   }
#endif

   for(i = 13; i < buf_size-SAMPLES_PER_PULSE; i++){

     /*find the trigger and if found, load data into the 2-d array,
       while keeping track of the time of the pulse*/
     if (start[i] == 1 && none(start,i-11,i-1) == 0){
       rdata_t_cpy(response_parsed[count], &response[i], SAMPLES_PER_PULSE);
       count = count + 1;
     }

     /*only record for a preset amount of triggers*/
     if (count == NUM_TRIGGERS)
       break;

   }

#ifdef PRINT_PARSED

   for (i = 0; i < NUM_TRIGGERS; i++){
     for (j = 0; j < SAMPLES_PER_PULSE/10; j++) {
       printf("%d ", (int)floorf(response_parsed[i][j]));
     }
     printf("\n");
   }
#endif

   /*subtract the avarage value of each sub array!*/
   for(i = 0; i < NUM_TRIGGERS; i++){
       average = mean(response_parsed[i], 0, SAMPLES_PER_PULSE-1);
       rdata_t_addi(response_parsed[i], -1.0*average, SAMPLES_PER_PULSE);
   }


#ifdef PRINT_AVERAGED

   for (i = 0; i < NUM_TRIGGERS; i++){
     for (j = 0; j < SAMPLES_PER_PULSE/10; j++) {
       printf("%d ", (int)floorf(response_parsed[i][j]));
     }
     printf("\n");
   }
#endif



   /*create 2-pulse cancelor*/
   for (i = 1; i < NUM_TRIGGERS; i++)
       sub_array(response_parsed[i], response_parsed[i-1], SAMPLES_PER_PULSE);


#ifdef PRINT_CANCELOR

   for (i = 0; i < NUM_TRIGGERS; i++){
     for (j = 0; j < SAMPLES_PER_PULSE/10; j++) {
       printf("%d ", (int)floorf(response_parsed[i][j]));
     }
     printf("\n");
   }
#endif


   /*ifft and convert to intensity values*/
   for(i = 0; i < NUM_TRIGGERS; i++){
       ifft(ifft_array,response_parsed[i]);
       rdata_t_cpy(response_parsed[i], ifft_array, size_of_sendarray);
       dbv(response_parsed[i], size_of_sendarray);
   }

#ifdef PRINT_IFFT
   for (i = 0; i < NUM_TRIGGERS; i++){
     for (j = 0; j < SAMPLES_PER_PULSE/10; j++) {
       printf("%d ", (int)floor(response_parsed[i][j]));
     }
     printf("\n");
   }
#endif


   max = find_max(&response_parsed[0][0], NUM_TRIGGERS*size_of_sendarray);

   rdata_t_addi(&response_parsed[0][0], -1.0*max, NUM_TRIGGERS*size_of_sendarray);

   intensify(intensity_time, &(response_parsed[0][0]), size_of_sendarray*NUM_TRIGGERS);


#ifdef PRINT_FINAL
   for (i = 0; i < NUM_TRIGGERS; i++){
     for (j = 0; j < SAMPLES_PER_PULSE/10; j++) {
         printf("%d ", (unsigned int)(unsigned char)floor(intensity_time[NUM_TRIGGERS*i+j]));
     }
     printf("\n");
   }
#endif



}


