/*
	Note: The following was borrow from the following Gist:
	https://gist.github.com/albanpeignier/104902
	
	It has been modified slightly to function as a library instead of a main executable.
	This should eventually be re-written to fit our needs more directly.
*/

/* 
	A Minimal Capture Program

	This program opens an audio interface for capture, configures it for
	stereo, 16 bit, 44.1kHz, interleaved conventional read/write
	access. Then its reads a chunk of random data from it, and exits. It
	isn't meant to be a real program.

	From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html

	Fixes rate and buffer problems

	sudo apt-get install libasound2-dev
	gcc -o alsa-record-example -lasound alsa-record-example.c && ./alsa-record-example hw:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "alsa.h"

static snd_pcm_t *capture_handle;
unsigned int rate = 44100;
snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
				
int init_alsa_device(char *device)
{
	int err;
	snd_pcm_hw_params_t *hw_params;

	if ((err = snd_pcm_open(&capture_handle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		fprintf(stderr, "cannot open audio device %s(%s)\n", device, snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "audio interface opened\n");
			 
	if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
		fprintf(stderr, "cannot allocate hardware parameter structure(%s)\n",snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params allocated\n");
				 
	if ((err = snd_pcm_hw_params_any(capture_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot initialize hardware parameter structure(%s)\n", snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params initialized\n");
	
	if ((err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf(stderr, "cannot set access type(%s)\n", snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params access setted\n");
	
	if ((err = snd_pcm_hw_params_set_format(capture_handle, hw_params, format)) < 0) {
		fprintf(stderr, "cannot set sample format(%s)\n", snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params format setted\n");
	
	if ((err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0)) < 0) {
		fprintf(stderr, "cannot set sample rate(%s)\n", snd_strerror(err));
		exit(1);
	}
	
	fprintf(stdout, "hw_params rate setted\n");

	if ((err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 2)) < 0) {
		fprintf(stderr, "cannot set channel count(%s)\n", snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params channels setted\n");
	
	if ((err = snd_pcm_hw_params(capture_handle, hw_params)) < 0) {
		fprintf(stderr, "cannot set parameters(%s)\n", snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "hw_params setted\n");
	
	snd_pcm_hw_params_free(hw_params);

	fprintf(stdout, "hw_params freed\n");
	
	if ((err = snd_pcm_prepare(capture_handle)) < 0) {
		fprintf(stderr, "cannot prepare audio interface for use(%s)\n", snd_strerror(err));
		exit(1);
	}

	fprintf(stdout, "audio interface prepared\n");

	return 0;
}

int deinit_alsa(void)
{
	snd_pcm_close(capture_handle);
	return 0;
}

// Note: Buffer must be of size (ALSA_FRAMES * width(format) / 8 * 2
int read_alsa_data(char * buffer)
{
	int err;

	if ((err = snd_pcm_readi(capture_handle, buffer, ALSA_FRAMES)) != ALSA_FRAMES) {
		fprintf(stderr, "read from audio interface failed(%s)\n", snd_strerror(err));
		return -1;
	}
	return 0;
}


