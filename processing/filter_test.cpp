#include <fstream>
#include <iostream>
#include "filter.h"
#include "../audio/sampling.h"

int main() {
  std::ifstream filter_coefs;
  std::ofstream non_filtered;
  std::ofstream filtered;
  filtered.open("filtered");
  non_filtered.open("non_filtered");
  filter_coefs.open("coef");
  filter F(filter_coefs);
  filter null_filter;
  int M = F.M;
  while(1) {
    amplitude_probes amp = amplitude_probes(16000, 4096, 2);
    amp.capture(1);
    pthread_mutex_lock(&(amp.is_done));
    /*for (int i = M; i < amp.count; i++) {
      non_filtered << amp[i] << std::endl;
      } */
    F.apply(amp);
    //null_filter.apply(amp);
    /*for (int i = M ; i < amp.count; i++) {
      filtered << amp.filtered[i] << std::endl;
      }*/
    amp.yin(0.2,1102,0); //it creats d_t_tau (t=0) //551 * 2 = 1102
    //return(0);
    for (int i = 1; i < 1102; i++) {
      printf("%.2f\n", amp.yin(0.1, 1102, i));
    }
  }
}
