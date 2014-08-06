
#ifndef RADAR_CONFIG_H
#define RADAR_CONFIG_H

/*radar parameters*/
#define SAMPLING_RATE (44100.0)
#define PULSE_TIME (20.0*0.001)
#define SAMPLES_PER_PULSE (((int)(PULSE_TIME*SAMPLING_RATE))/1)
#define NUM_TRIGGERS (5)

#define DATA_BUFFER_SIZE ((NUM_TRIGGERS+1)*SAMPLES_PER_PULSE)

#define FREQ_START (2260.0*1000000.0)
#define FREQ_END (2590.0*1000000.0)
#define BAND_WIDTH (FREQ_END - FREQ_START)

#define RANGE_RESOLUTION (3*100000000/(2*BAND_WIDTH))
#define MAX_RANGE (RANGE_RESOLUTION*SAMPLES_PER_PULSE/2.0)

#define THRESHOLD (0)

#define TRUE (1)
#define FALSE (0)

char* dump_buffer;
int size_of_sendarray;

#endif
