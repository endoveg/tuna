#include <fstream>
#include <iostream>
#include "filter.h"
#include "../audio/sampling.h"
#include "yin.h"
double last_found_freq = 440.0;

int main() {
  std::ifstream filter_coefs;
  filter_coefs.open("coef");
  filter F(filter_coefs);
  for(int i = 0; i < 100; i++) {
    amplitude_probes *amp = new amplitude_probes(22050, 4096, 2);
    amp->capture(1);
    pthread_mutex_lock(&(amp->is_done));
    F.apply(amp);
    yin(amp, 0.2,1102,0); //it creats d_t_tau (t=0) //551 * 2 = 1102
    for (int i = 1; i < 1102; i++) {
      printf("%.2f\n", yin(amp, 0.2, 1102, i));
    }
  }
}
