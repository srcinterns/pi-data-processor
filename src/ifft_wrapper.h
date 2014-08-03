#ifndef IFFT_WRAPPER_H
#define IFFT_WRAPPER_H

int init_fft(int buf_size);
void end_fft();
void ifft(float* time_domain, float* freq_domain);



#endif
