#include "sampling.h"
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

int main() {
  amplitude_probes amp = amplitude_probes(44100, (unsigned long int) 16384, 2);
  syncmaster *sync0;
  sync0 = amp.capture(1);
  while (pthread_mutex_trylock(&(sync0->is_done)) == EBUSY) {
        printf("I am waiting\n");
  }
  return(0);
}
