/*******************************************************/
/* ifft_wrapper.c - David Campbell
 *  8/2/2014
 *  personal email: dncswim76@gmail.com
 *  src email dcampbell@srcinc.com
 *
 *  This file contains code that transfers the fftw
 *  interface into a much easier function to work with.
 *  The init_fft plan should be called on program start
 *  up and the end_fft should be called on close.  Finally
 *  ifft function can be easily called to do an ifft
 *  on two rdata_t arrays.  This ifft will only create
 *  real time domain data                               */
/********************************************************/


#include <fftw3.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "radar_config.h"

static fftw_complex* in;
static fftw_complex* out;
static int N;
static fftw_plan p;


/*INIT_FFT - set up the ifft environment,
 * the buf size is the length of the two
 * arrays that the ifft will be working
 * on                                 */
int init_fft(int buf_size){
  N = (int)pow(2, ceil(log2((double)buf_size)));
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
    return N;
}

/*END FFT - end the ifft environment*/
void end_fft(){

    fftw_destroy_plan(p);
    free(in);
    free(out);
}

/* RDATA_T TO DOUBLE - convert an array of rdata_ts
 * to an array of doubles of size N (stored globally)*/
void flt2dbl(fftw_complex* array_complex, rdata_t* array_flt){
    int i;
    assert(array_complex != NULL);
    assert(array_flt != NULL);

    for (i = 0; i < N; i++){
        array_complex[i][0] = (double)array_flt[i];
        array_complex[i][1] = 0;
    }
}

/*DOUBLE TO RDATA_T - convert a double array to a
 * rdata_t array of size N (stored globally)*/
void dbl2flt(rdata_t* array_flt, fftw_complex* array_complex){

    int i;
    double A, B;

    assert(array_complex != NULL);
    assert(array_flt != NULL);

    for (i = 0; i < N; i ++){
        /*we need to divide by N because the fftw library doesn't
         *do this*/
        A = array_complex[i][0]/N;
        B = array_complex[i][1]/N;
        array_flt[i] = (rdata_t)sqrt( A*A + B*B);
     }
}

/*IFFT - copy the data into the ifft buffer,
 * execute the ifft, then copy it to the output
 * buffer*/
void ifft(rdata_t* time_domain, rdata_t* freq_domain){

    flt2dbl(in, freq_domain);

    fftw_execute(p);

    dbl2flt(time_domain, out);
}
