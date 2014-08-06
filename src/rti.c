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

#define PRINT_TRIGGERING
//#define PRINT_PARSED

static char* start;
static float* ifft_array;

/* FIND_TRIGGER_START - given the trigger array, if the data is greater
 * than a certain threshold, mark in the "start" array that value is
 * greater than the threshold.  Both arrays are meant to be of array size.*/
void find_trigger_start(float* trigg_array, char* start, int array_size){
   int i;

   assert(trigg_array != NULL);
   assert(start != NULL);

   printf("Processing Trigger Start...");

   for (i = 0; i < array_size; i++) {

     if (trigg_array[i] > THRESHOLD){
	start[i] = 1;
     }else
	start[i] = 0;

   }

   printf("Trigger Start Processed\n");
   return;
}

float char_mean(char* array, int start, int stop){

   int i;
   float sum;
   float count = 0;

   assert(array != NULL);

   for (i = start; i <= stop; i++){
     sum = sum + (float)array[i];
     count++;
   }
   return sum/count;

}

/* MEAN - find the average of the array over
*   the specified interval.  Both start and
*   stop are inclusive                    */
float mean(float* array, int start, int stop){

   int i;
   float sum;
   float count = 0;

   assert(array != NULL);

   for (i = start; i <= stop; i++){
     sum = sum + array[i];
     count++;
   }
   return sum/count;

}

/*ABS - find the absolute value of a float*/
float abs_value(float value){
  if (value < 0.0)
    return -1.0*value;
  return value;
}

/*FLOAT COPY - copy the elements from one float array
 * to another specified by a certain amount*/
void float_cpy(float* arrayA, float* arrayB, int amount){
  int i;

  assert(arrayA != NULL);
  assert(arrayB != NULL);

  for (i = 0; i < amount; i++){
    arrayA[i] = arrayB[i];
  }
}


/*FLOAT ADD IMMEDIATE - add a scalar value
 * from all the elements in the float array*/
void float_addi(float* arrayA, float value, int size){
  int i;

  assert(arrayA != NULL);

  for (i = 0; i < size; i++){
    arrayA[i] = arrayA[i] + value;
  }
}

/*SUBTRACT ARRAY - arrayA[i] = arrayB[i] - arrayA[i]*/
void sub_array(float* arrayA, float* arrayB, int size){
    int i;

    assert(arrayA != NULL);
    assert(arrayB != NULL);

    for (i = 0; i < size; i++)
        arrayA[i] = arrayB[i] - arrayA[i];
}

/*INTENSIFY - make data into a value from 0 to 255*/
void intensify(char* char_array, float* float_array, int size){
  int i;
  float temp_float;
  int temp;

  assert(char_array != NULL);
  assert(float_array != NULL);

  for (i = 0; i < size; i++){
    temp_float  = float_array[i] + 80;

    if (temp_float < 0)
        temp_float = 0;

    temp = floorf(((temp_float/80.0)*255));

    if (temp > 255)
      temp = 255;

    char_array[i] = (char) temp;
  }
}


/*FIND MAX - find that maximum value in the
 * array*/
float find_max(float* array, int size){
    int i;
    float max = 0;

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
    ifft_array = calloc(size_of_sendarray, sizeof(float));

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
                      float* trigger, float* response, int buf_size){

   int count = 0;
   int i, j;
   float average, max;

   assert(intensity_time != NULL);
   assert(trigger != NULL);
   assert(response != NULL);

   /*for size of radar data to be correct, init_processing must be
    *called before this function                                */
   float response_parsed[NUM_TRIGGERS][size_of_sendarray];

   /*create a simplied edge trigger based off the transmit signal*/
   memset(start, 0, buf_size);
   find_trigger_start(trigger, start, buf_size);

#ifdef PRINT_TRIGGERING
   for (i = 0; i < buf_size; i++) {
     printf("%d: %d %f\n", i, start[i], trigger[i]);
   }
#endif

   for(i = 13; i < buf_size-SAMPLES_PER_PULSE; i++){

     /*find the trigger and if found, load data into the 2-d array,
       while keeping track of the time of the pulse*/
      if (start[i] == 1 && char_mean(start,i-11,i-1) == 0){

          float_cpy(response_parsed[count], &response[i], SAMPLES_PER_PULSE);
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
       float_addi(response_parsed[i], -1.0*average, SAMPLES_PER_PULSE);
   }

   /*create 2-pulse cancelor*/
   for (i = 1; i < NUM_TRIGGERS; i++)
       sub_array(response_parsed[i], response_parsed[i-1], SAMPLES_PER_PULSE);

   /*ifft and convert to intensity values*/
   for(i = 0; i < NUM_TRIGGERS; i++){
       ifft(ifft_array,response_parsed[i]);
       float_cpy(response_parsed[i], ifft_array, size_of_sendarray);
       dbv(response_parsed[i], size_of_sendarray);
   }

#ifdef PRINT_IFFT
   for (i = 0; i < NUM_TRIGGERS; i++){
     for (j = 0; j < SAMPLES_PER_PULSE/10; j++) {
       printf("%d ", (int)floorf(response_parsed[i][j]));
     }
     printf("\n");
   }
#endif


   max = find_max(&response_parsed[0][0], NUM_TRIGGERS*size_of_sendarray);

   float_addi(&response_parsed[0][0], -1.0*max, NUM_TRIGGERS*size_of_sendarray);

   intensify(intensity_time, &(response_parsed[0][0]), size_of_sendarray*NUM_TRIGGERS);

}


