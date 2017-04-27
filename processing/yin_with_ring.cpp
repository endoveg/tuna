#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <complex>
#include "dft.h"
#include <fstream>
#include <signal.h>
#include "../audio/sampling.h"
#include "ring_buff.h"
#include "yin.h"
#include "filter.h"

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
sem_t count_sem, space_sem;
filter *F;
double last_found_freq = 440.0;
bool state;

//this file's purpose is only to show
//how to initialize and destroy mutex and semaphores,
//how to use template ring buffer

void* writer (ring_buffer <amplitude_probes>* ptr)
{
    amplitude_probes* amp = new amplitude_probes(22050, 4096, 2);
    if (amp == NULL) {
      printf("wtf?");
      exit(1);
    }
    amp->capture (1);
    pthread_mutex_lock(&amp->is_done);
    F->apply(amp);
    ptr->write(amp);
    return NULL;
}

void* reader (ring_buffer <amplitude_probes>* ptr)
{
  ptr->read (yin_tuner, 0.1, 1101);
  return NULL;
}

void switcher (int foo)
{
    state = false;
}

void * wt (void * p) {
  ring_buffer <amplitude_probes> *a = (ring_buffer <amplitude_probes> *) p;
  while(state)
  {
    signal (3, switcher);
    writer(a);
  }
}
void * rt (void *p) {
  ring_buffer <amplitude_probes> *a = (ring_buffer <amplitude_probes> *) p;
  while(state)
  {
    signal (3, switcher);
    reader(a);
  }
}

int main ()
{
    const unsigned int buff_size = 16;
    state = true;
    pthread_mutex_init (&mtx, NULL);
    sem_init (&count_sem, 0, 0);
    sem_init (&space_sem, 0, buff_size);
    ring_buffer <amplitude_probes>* a = new ring_buffer <amplitude_probes> (buff_size);
    std::ifstream filter_coefs;
    filter_coefs.open("coef");
    F = new filter(filter_coefs);
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
    return 0;
}
