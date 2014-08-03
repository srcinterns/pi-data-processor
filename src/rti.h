
#ifndef RTI_H
#define RTI_H

void init_processing();
void clean_up_processing();
void process_radar_data(char* intensity_time,
                        float* trigger, float* response, int buf_size);

#endif;
