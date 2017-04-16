#include "../audio/sampling.h"
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
int main() {
  amplitude_probes amp = amplitude_probes(44100, (unsigned long int) 16384, 2);
  amp.capture(1);
  while (pthread_mutex_trylock(&(amp.is_done)) == EBUSY) {
    //    printf("I am waiting\n");
  }
  amp.yin(0.1,16384/2);
  return 0;
}
