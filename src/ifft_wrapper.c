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
 *  on two float arrays.  This ifft will only create
 *  real time domain data                               */
/********************************************************/


#include <fftw3.h>
#include <stdlib.h>
#include <assert.h>

static double* in;
static double* out;
static int N;
fftw_plan p;


/*INIT_FFT - set up the ifft environment,
 * the buf size is the length of the two
 * arrays that the ifft will be working
 * on                                 */
void init_fft(int buf_size){
    N = buf_size;
    in = (double*) fftw_malloc(sizeof(double) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(double) * N);
    p = fftw_plan_r2r_1d(N, in, out, FFTW_BACKWARD, FFTW_ESIMATE);
}

/*END FFT - end the ifft environment*/
void end_fft(){

    fftw_destroy_plan(p);
    free(in);
    free(out);
}

/* FLOAT TO DOUBLE - convert an array of floats
 * to an array of doubles of size N (stored globally)*/
void flt2dbl(double* array_dbl, float* array_flt){
    int i;
    assert(array_dbl != NULL);
    assert(array_flt != NULL);

    for (i = 0; i < N, i++)
        array_dbl[i] = (double)array_flt[i];
}

/*DOUBLE TO FLOAT - convert a double array to a
 * float array of size N (stored globally)*/
void dbl2flt(float* array_flt, double* array_dbl){

    int i;

    assert(array_dbl != NULL);
    assert(array_flt != NULL);

    for (i = 0; i < N, i ++)
        /*we need to divide by N because the fftw library doesn't
         *do this*/
        array_flt[i] = (float)array_dbl[i]/((float)N);
}

/*IFFT - copy the data into the ifft buffer,
 * execute the ifft, then copy it to the output
 * buffer*/
void ifft(float* time_domain, float* freq_domain){

    flt2dbl(in, freq_domain);

    fftw_execute(p);

    dbl2flt(time_domain, out);
}