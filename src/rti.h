
#ifndef RTI_H
#define RTI_H

#include "radar_config.h"

void init_processing();
void clean_up_processing();
void process_radar_data(char* intensity_time,
                        rdata_t* trigger, rdata_t* response, int buf_size);

#endif
