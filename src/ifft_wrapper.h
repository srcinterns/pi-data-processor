#ifndef IFFT_WRAPPER_H
#define IFFT_WRAPPER_H

void init_fft(int buf_size);
void end_fft();
void ifft(float* time_domain, float* freq_domain);



#endif
