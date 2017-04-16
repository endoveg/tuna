#include "../audio/sampling.h"
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
int main() {
  unsigned long int length = 20000;
  amplitude_probes amp = amplitude_probes(44100, length, 2);
  amp.capture(1);
  pthread_mutex_lock(&(amp.is_done));
  double freq;
  unsigned int time;
  unsigned int upper_bound = length / 2048;
  amp.yin(0.15,2048,228);
  //printf("%2.f", amp.yin(0.1, 2048,57));
  /* for (time = 0; time < upper_bound; time++) {
     printf("%.2f\n", amp.yin(0.3,2048,time * 2048));
     }*/
  return 0;
}
