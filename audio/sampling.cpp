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
syncmaster * amplitude_probes::capture(int num_for_key){
  pthread_t ptid;
  int pth;
  key_t key = ftok("nicole_boorbaki", num_for_key);
  if (key == -1) {
    printf("%s\n", strerror(errno));
    return(NULL);
  }
  syncmaster *p = (syncmaster *)malloc(sizeof(syncmaster));
  p->is_done = PTHREAD_MUTEX_INITIALIZER;
  p->shared_meme_id = shmget(key, count * bits_per_sample,
			     IPC_CREAT | S_IRWXU);
  if (p->shared_meme_id == -1) {
    printf("%s\n", strerror(errno));
    return(NULL);
  }
  pthread_mutex_lock(&(p->is_done));
  sampling_thread_arg *arg = (sampling_thread_arg *)malloc(sizeof(sampling_thread_arg));
  arg->audio_stream = "plughw:1,0";
  arg->amp_params = this;
  arg->nsync = p;
  pth = pthread_create(&ptid, NULL, &sampling_thread, (void *) arg);
  amplitudes = shmat(p->shared_meme_id, NULL, 0);
  return p;
}
void * sampling_thread(void * arg) {
  sampling_thread_arg * ARG;
  ARG = (sampling_thread_arg *) arg;
  int i;
  int err;
  char *buffer;
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

  //fprintf(stdout, "audio interface opened\n");
		   
  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  //fprintf(stdout, "hw_params allocated\n");
				 
  if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  //fprintf(stdout, "hw_params initialized\n");
	
  if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  //fprintf(stdout, "hw_params access setted\n");
	
  if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
    fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  //fprintf(stdout, "hw_params format setted\n");
	
  if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
    fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
    exit (1);
  }
	
  //fprintf(stdout, "hw_params rate setted\n");

  if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 1)) < 0) {
    fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  //fprintf(stdout, "hw_params channels setted\n");
	
  if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  //fprintf(stdout, "hw_params setted\n");
	
  snd_pcm_hw_params_free (hw_params);

  //fprintf(stdout, "hw_params freed\n");
	
  if ((err = snd_pcm_prepare (capture_handle)) < 0) {
    fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  //fprintf(stdout, "audio interface prepared\n");

  buffer = (char *) shmat(ARG->nsync->shared_meme_id, NULL, 0);

  //  fprintf(stdout, "buffer allocated\n");

  if ((err = snd_pcm_readi (capture_handle, buffer, buffer_frames)) != buffer_frames) {
    fprintf (stderr, "read from audio interface failed (%s)\n",
	      snd_strerror (err));
    exit (1);
  }
  //fprintf(stdout, "read %d done\n", i);

  //fprintf(stdout, "buffer freed\n");
	
  snd_pcm_close (capture_handle);
  //fprintf(stdout, "audio interface closed\n");
  pthread_mutex_unlock(&(ARG->nsync->is_done));
  return NULL;
}
