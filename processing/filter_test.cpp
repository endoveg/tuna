#include <fstream>
#include <iostream>
#include "filter.h"
#include "../audio/sampling.h"
#include "yin.h"
double last_found_freq = 440.0;

int main() {
  std::ifstream filter_coefs;
  std::ofstream non_filtered;
  std::ofstream filtered;
  filtered.open("filtered.out");
  non_filtered.open("non_filtered.out");
  filter_coefs.open("coef");
  filter F(filter_coefs);
  while(1) {
    amplitude_probes *amp = new amplitude_probes(44100, 4096, 2);
    amp->capture(1);
    pthread_mutex_lock(&(amp->is_done));
    int M = 11;
    for (int i = M; i < amp->count; i++) {
      non_filtered << (*amp)[i] << std::endl;
    }
    F.apply(amp);
    for (int i = M ; i < amp->count; i++) {
      filtered << amp->filtered[i] << std::endl;
      }
    return(0);
    yin(*amp,0.2,1102,0); //it creats d_t_tau (t=0) //551 * 2 = 1102
    return(0);
    //return(0);
    for (int i = 1; i < 1102; i++) {
      printf("%.2f\n", yin(*amp, 0.2, 1102, i));
    }
  }
}
