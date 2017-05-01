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
void amplitude_probes::capture(audio_handler& audio){
  amplitudes = malloc( count * bits_per_sample);
  int err;
  int read_size = 1024;
  int cur = 0;
  if (amplitudes == NULL) {
      fprintf(stderr, "problem with memory\n");
      exit(1);
  }
  while(cur < count)  {
    if ((err = snd_pcm_readi (audio.capture_handle,
                  (char *)amplitudes + cur*bits_per_sample,
                              audio.read_size)) != audio.read_size) {
      if (err = -EPIPE) {
	snd_pcm_recover(audio.capture_handle, err, 0);
	audio.read_size = audio.read_size / 4;
      } else {
	fprintf(stderr, "error while reading\n");
      }
    } else {
      cur = cur + audio.read_size;
    }
  }
}

void audio_handler::open() {
  int err;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_format_t format;
  format = SND_PCM_FORMAT_S16_LE;
  if (bits_per_sample == 2) {
      format = SND_PCM_FORMAT_S16_LE;
  }

  if ((err = snd_pcm_open (&capture_handle, audio_device, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n", 
             audio_device,
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
}

void audio_handler::close() {
  snd_pcm_close (capture_handle);
}

