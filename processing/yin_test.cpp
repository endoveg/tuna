#include "../audio/sampling.h"
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
int main() {
  unsigned long int length =  2048 + 2048 + 7; //prozapas;
  /*while(1){
    amplitude_probes amp = amplitude_probes(44100, length, 2);
    amp.capture(1);
    pthread_mutex_lock(&(amp.is_done));
    printf("\e[2J");
    printf("%.2f\n", amp.yin(0.1, 2048,0));
    }*/
  while(1) {
    amplitude_probes amp = amplitude_probes(44100, length, 2);
    amp.capture(1);
    pthread_mutex_lock(&(amp.is_done));
    amp.yin(0.1,1024,0); //it creats d_t_tau (t=0)
    for (int i = 1; i < 1024; i++) {
      printf("\e[2J");
      printf("%.2f\n", amp.yin(0.2, 1024,i));
      }
  }
  return 0;
}
