#ifndef ALSA_FRAMES
#error ALSA_FRAMES not defined!
#endif

#define ALSA_BUFFER_SIZE (ALSA_FRAMES*snd_pcm_format_width(format)/8*2)
int init_alsa_device(char * device);
int read_alsa_data(char * buffer);
int deinit_alsa(void);
