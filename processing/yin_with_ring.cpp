#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <complex>
#include <fstream>
#include <signal.h>
#include "filter.h"
#include "../audio/sampling.h"
#include "ring_buff.h"
#include "yin.h"
#include "pitch_detection.h"
#include "dft.h"

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
sem_t count_sem, space_sem;
filter *F;
double last_found_freq = 440.0;
bool state;
audio_handler *audio;
const int N = 3072;
const double f_d = 5000;
//this file's purpose is only to show
//how to initialize and destroy mutex and semaphores,
//how to use template ring buffer

void switcher (int a)
{
    state = false;
}

void * wt (void * p) {
  ring_buffer <amplitude_probes> *ptr = (ring_buffer <amplitude_probes> *) p;
  while(state)
  {
    signal (3, switcher);
    amplitude_probes* amp = new amplitude_probes(f_d, N, 2);
    amp->capture (*audio);
#ifdef YIN    
    F->apply(amp);
#endif    
    ptr->write(amp);
  }
}
void * rt (void *p) {
  ring_buffer <amplitude_probes> *ptr = (ring_buffer <amplitude_probes> *) p;
#ifndef YIN
  unsigned int wsize = 2048;
  std::vector<std::complex <double> > im (wsize / 2);
  unsigned int shift = 3;
  std::vector<std::complex <double> > expos (shift * wsize / 2);
  std::vector<double> freq (wsize / 2);
  std::vector<double> ampl (wsize / 2);
  for (unsigned int i = 0; i < shift * wsize / 2; i++)
  {
      expos [i] = exp (std::complex <double> (0, 2 * M_PI * i / wsize));
      if (i < wsize / 2)
          freq [i] = i * f_d / (double)wsize;
  }
#endif
  while(state)
  {
    signal (3, switcher);
#ifdef YIN
    ptr->read (yin_tuner, 0.1, 1101);
#else
    ptr->read (processor, im, expos, ampl, wsize, f_d, freq, shift);
#endif
  }
}

int main ()
{
  audio = new audio_handler(f_d, 2, "plughw:1,0");
  audio->open();
  const unsigned int buff_size = 16;
  state = true;
  pthread_mutex_init (&mtx, NULL);
    sem_init (&count_sem, 0, 0);
    sem_init (&space_sem, 0, buff_size);
    ring_buffer <amplitude_probes>* a = new ring_buffer <amplitude_probes> (buff_size);
#ifdef YIN
    std::ifstream filter_coefs;
    filter_coefs.open("coef");
    F = new filter(filter_coefs);
#endif
    pthread_t writing_thread;
    pthread_t reading_thread;
    int pth;
    pth = pthread_create(&writing_thread, NULL, &wt, (void *) a);
    pth = pthread_create(&reading_thread, NULL, &rt, (void *) a);
    pth = pthread_join(writing_thread, NULL);
    pth = pthread_join(reading_thread, NULL);
    pthread_mutex_destroy (&mtx);
    sem_destroy (&count_sem);
    sem_destroy (&space_sem);
    delete a;
    audio->close();
    return 0;
}
