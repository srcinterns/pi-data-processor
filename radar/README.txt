from DAVE:

radar_processing.h

//data to send is a 2-d arra, preset to 0
void process_radar_data(char* data_to_send, float* trigger, float* response);

to put header file:


#NUM_PULSES - amount of time arrays "columns"
#NUM_SAMPLES_PER_PULSE - amount of samples in a time period 

#radar_config.h

-> array[NUM_PULSES][NUM_SAMPLES_PER_PULSE]

-> DATA_BUFFER_SIZE - total size of buffer that stores audio data


return the size of the time_domain
-freq domain all real values

ifft_wrapper.h

Zach or Eric:
int ifft(float* frequency_doamin, float* time_domain, int size_of_freq_domain);

dbv.h

Zach:
// take 20*log10(array[i])
void dbv(float* array, int size)
