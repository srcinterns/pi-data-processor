
#include <sys/time.h> 
#include <stdlib.h>

/*radar parameters*/
#define SAMPLING_RATE (1000000.0)
#define PULSE_TIME (20.0*0.001)
#define SAMPLES_PER_PULSE (((int)(PULSE_TIME*SAMPLING_RATE))/1)
#define NUM_TRIGGERS (500)

#define DATA_BUFFER_SIZE ((NUM_TRIGGERS+1)*SAMPLES_PER_PULSE)

#define FREQ_START (2260.0*1000000.0)
#define FREQ_END (2590.0*1000000.0)
#define BAND_WIDTH (FREQ_END - FREQ_START)

#define RANGE_RESOLUTION (3*100000000/(2*BAND_WIDTH))
#define MAX_RANGE (RANGE_RESOLUTION*SAMPLES_PER_PULSE/2.0)

#define THRESHOLD (0)
#define MAX_INTENSITY (65535) //2^16-1

#define TRUE (1)
#define FALSE (0)



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
    arrayA[i] = abs_value(arrayB[i]);
  }
}


/*FLOAT SUBTRACT IMMEDIATE - subtract a scalar value
 * from all the elements in the float array*/
void float_subi(float* arrayA, float value, int size){
  int i;
  for (i = 0; i < size; i++){
    arrayA[i] = arrayA[i] - value;
  }
}

/*INTENSIFY - make data into a value from 0 to 255*/
void intensify(char* char_array, float* float_array, int size){
  int i;
  int temp;
  for (i = 0; i < size; i++){
    temp  = (int)(float_array[i] / MAX_INTENSITY * 255.0);
    if (temp > 255)
      temp = 255;
    char_array[i] = (char) temp;
  }
}

/*COMPACT - avarage over a time slice relative to pulse
 * to compact data*/
void compact(char single_array[SAMPLES_PER_PULSE], 
	     char double_array[NUM_TRIGGERS][SAMPLES_PER_PULSE], 
	     int size_of_double, int array_size){
  
  int i,j;
  int sum;
  for (j = 0; j < array_size; j++){
    sum = 0;
    for (i = 0; i < size_of_double; i++){
      sum = sum + double_array[i][j];
    }
    single_array[j] = sum/size_of_double;
  }
}

/*PARSE RADAR DATA - traverse the two data array's and use the start_array to determine
 * the start of a pulse.  Keep track of the pulse, and log the pulse data based on the 
 * pulse count into a 2 dimensional array, response_parsed*/ 
void parse_radar_data(char* intensity_time, float* trigger, float* response, int buf_size){
   int count = 0;
   int i;
   float average;
   float start[DATA_BUFFER_SIZE];
   float response_parsed[NUM_TRIGGERS][SAMPLES_PER_PULSE];
   char intensity_value[NUM_TRIGGERS][SAMPLES_PER_PULSE];

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
   for(i = 0; i < NUM_TRIGGERS; i++){
     average = mean(response_parsed[i], 0, SAMPLES_PER_PULSE-1);
     float_subi(response_parsed[i], average, SAMPLES_PER_PULSE);
   }

   /*transfer it to intensity values*/
   for(i = 0; i < NUM_TRIGGERS; i++)
     intensify(intensity_value[i], response_parsed[i], SAMPLES_PER_PULSE);

}

/*PROCESS_RADAR_DATA - endlessly extract the radar data, 
 * process it, then messagize it over ethernet*/
unsigned long process_radar_data(char** send, float* trigger, float* response){

  float global_response[DATA_BUFFER_SIZE];
  float global_trigger[DATA_BUFFER_SIZE];
  char global_send_data[SAMPLES_PER_PULSE];


  struct timeval tv;
  gettimeofday(&tv,NULL);

  //should be 32 but unsigned integer
  unsigned long time_in_micros_start = 1000000 * tv.tv_sec + tv.tv_usec;
  unsigned long time_in_micros_end;
  int timestamp;

     
    gettimeofday(&tv, NULL);
    //unsigned long time_in_micros_end = 1000000 * tv.tv_sec + tv.tv_usec;

    /*read the data from the audio port*/
  
    parse_radar_data(global_send_data, global_trigger, global_response, DATA_BUFFER_SIZE);

    timestamp = (int)(time_in_micros_end - time_in_micros_start);
    
    //send_udp_message(timestamp, send_data, SAMPLES_PER_PULSE)

  

}


      
