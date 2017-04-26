#include "sampling.h"
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

int main() {
  audio_handler *audio = new audio_handler(44100, 2, "plughw:1,0");
  audio->open();
  while(1) {
    amplitude_probes *amp = new amplitude_probes(44100, (unsigned long int) 16384, 2);
    amp->capture(*audio);
    int i;
    for (i = 0; i < 16384; i++) {
      //  printf("%d\n", (*amp)[i]);
    }
    delete amp;
  }
  return(0);
}
