#include "sampling.h"
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

int main() {
  amplitude_probes *amp = new amplitude_probes(44100, (unsigned long int) 16384, 2);
  amp->capture(1);
  while (pthread_mutex_trylock(&(amp->is_done)) == EBUSY) {
    //      printf("I am waiting\n");
  }
  int i;
  for (i = 0; i < 16384; i++) {
    printf("%ld\n", (*amp)[i]);
  }
  delete amp;
  return(0);
}
