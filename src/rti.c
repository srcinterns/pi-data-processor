#include <stdlib.h>
#include "radar_config.h"
#include "rti.h"


/* FIND_TRIGGER_START - if the value in the array is greater than a given
 *  threshold then change that value in the array to be 1*/
void find_trigger_start(float* trigg_array, float* start, int array_size){
   int i;
   for (i = 0; i < array_size; i++) {

      if (trigg_array[i] > THRESHOLD)
         start[i] = 1;
      else
          start[i] = 0;

   }
   return;
}

/*MEAN - find the average of the array over
* the specified interval.  Both start and
* stop are inclusive*/
float mean(float* array, int start, int stop){

   int i;
   float sum;
   float count = 0;
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
 * to another specified by a certain amount, and taking
 * the absolute value along the way!*/
void float_cpy(float* arrayA, float* arrayB, int amount){
  int i;
  for (i = 0; i < amount; i++){
    arrayA[i] = arrayB[i];
  }
}


/*FLOAT ADD IMMEDIATE - subtract a scalar value
 * from all the elements in the float array*/
void float_addi(float* arrayA, float value, int size){
  int i;
  for (i = 0; i < size; i++){
    arrayA[i] = arrayA[i] + value;
  }
}

/*SUBTRACT ARRAY - arrayA[i] = arrayB[i] - arrayA[i]*/
void sub_array(float* arrayA, float* arrayB, int size){
    int i;
    for (i = 0; i < size; i++)
        arrayA[i] = arrayB[i] - arrayA[i];
}

/*INTENSIFY - make data into a value from 0 to 255*/
void intensify(char* char_array, float* float_array, int size){
  int i;
  float temp_float;
  int temp;
  for (i = 0; i < size; i++){
    temp_float  = float_array[i] + 80;

    if (temp_float < 0)
        temp_float = 0;

    temp = round(((temp_float/80.0)*255));

    if (temp > 255)
      temp = 255;

    char_array[i] = (char) temp;
  }
}

float find_max(float* array, int size){
    int i;
    float max = 0;
    for ( i = 0; i < size; i++){
        if (array[i] > max)
            max = array[i];
    }

    return max;
}


/*PARSE RADAR DATA - traverse the two data array's and
 * use the start_array to determine
 * the start of a pulse.  Keep track of the pulse,
 * and log the pulse data based on the
 * pulse count into a 2 dimensional array, response_parsed*/
void process_radar_data(char* intensity_time,
                      float* trigger, float* response, int buf_size){
   int count = 0;
   int i;
   float average;
   float start[DATA_BUFFER_SIZE];
   float response_parsed[NUM_TRIGGERS][SAMPLES_PER_PULSE];
   float* ifft_array[SAMPLES_PER_PULSE];
   float max;

   /*create a simplied edge trigger based off the transmit signal*/
   find_trigger_start(trigger, start, buf_size);

   for(i = 13; i < buf_size-SAMPLES_PER_PULSE; i++){

     /*find the trigger and if found, load data into the 2-d array,
       while keeping track of the time of the pulse*/
      if (start[i] == 1 && mean(start,i-11,i-1) == 0){

          float_cpy(response_parsed[count], &response[i], SAMPLES_PER_PULSE);
          count = count + 1;
      }

      /*only record for a preset amount of triggers*/
      if (count == NUM_TRIGGERS)
	break;

   }

   /*subtract the avarage value*/
   average = mean(response_parsed[i], 0, SAMPLES_PER_PULSE-1);
   for(i = 0; i < NUM_TRIGGERS; i++){
     float_addi(response_parsed[i], -1.0*average, SAMPLES_PER_PULSE);
   }

   /*create 2-pulse cancelor*/
   for (i = 1; i < NUM_TRIGGERS; i++)
       sub_array(response_parsed[i], response_parsed[i-1], SAMPLES_PER_PULSE);

   /*ifft and convert to intensity values*/
   for(i = 0; i < NUM_TRIGGERS; i++){
       ifft(ifft_array,response_parsed[i],SAMPLES_PER_PULSE);
       float_cpy(response_parsed[i], ifft_array, SAMPLES_PER_PULSE);
       dbv(response_parsed[i], SAMPLES_PER_PULSE);
   }

   max = find_max(response_parsed, NUM_TRIGGERS*SAMPLES_PER_PULSE);
   array_addi(response_parsed, -1.0*max, NUM_TRIGGERS*SAMPLES_PER_PULSE);
   intensify(intensity_time, response_parsed, SAMPLES_PER_PULSE*NUM_TRIGGERS);

}


