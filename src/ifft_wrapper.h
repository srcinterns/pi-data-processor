#ifndef IFFT_WRAPPER_H
#define IFFT_WRAPPER_H

#include "radar_config.h"

int init_fft(int buf_size);
void end_fft();
void ifft(rdata_t* time_domain, rdata_t* freq_domain);



#endif
