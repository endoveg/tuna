#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include "sampling.h"
#include <errno.h>

void * sampling_thread(void * arg);
amplitude_probes::~amplitude_probes() {
  free(amplitudes);
  return;
}

double amplitude_probes::get(unsigned int index) {
  return filtered[index];
}

void amplitude_probes::set(unsigned int index, long int value) {
    /*
      There is also 24 float, but it's harder to "decode"
      1 byte = 8 bit
      2 bytes = 16 bit
     */
    switch (bits_per_sample) {
    case 1:
      *((int8_t *)amplitudes + index) = value;
      return;
    case 2:
      *((int16_t *)amplitudes + index) = value;
      return;
    }
}
void amplitude_probes::capture(int num_for_key){
  pthread_t ptid;
  int pth;
  is_done = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&is_done);
  amplitudes = malloc(rate * count * bits_per_sample);
  sampling_thread_arg *arg = (sampling_thread_arg *)malloc(sizeof(sampling_thread_arg));
  //arg->audio_stream = "plughw:1,0";
  //arg->audio_stream = "AUDIO_DEVICE";
  arg->audio_stream = "plughw:0,0";
  arg->amp_params = this;
  pth = pthread_create(&ptid, NULL, &sampling_thread, (void *) arg);
  return;	
}
void * sampling_thread(void * arg) {
  sampling_thread_arg * ARG;
  ARG = (sampling_thread_arg *) arg;
  int i;
  int err;
  int buffer_frames = ARG->amp_params->count;
  unsigned int rate = ARG->amp_params->rate;
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

  if ((err = snd_pcm_open (&capture_handle, ARG->audio_stream, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n", 
             ARG->audio_stream,
             snd_strerror (err));
    exit (1);
  }

		   
  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

				 
  if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

	
  if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
    exit (1);
  }

	
  if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
    fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
    exit (1);
  }

	
  if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
    fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
    exit (1);
  }
	

  if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
    fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
    exit (1);
  }

	
  if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }

	
  snd_pcm_hw_params_free (hw_params);

	
  if ((err = snd_pcm_prepare (capture_handle)) < 0) {
    fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
    exit (1);
  }


  

  if ((err = snd_pcm_readi (capture_handle, ARG->amp_params->amplitudes, buffer_frames)) != buffer_frames) {
    fprintf (stderr, "read from audio interface failed (%s)\n",
	     snd_strerror (err));
    exit (1);
  }
	
  snd_pcm_close (capture_handle);
  pthread_mutex_unlock(&(ARG->amp_params->is_done));
  return NULL;
}

