#include <iostream>
#include <vector>
#include <fstream>
#include "filter.h"
#include "../audio/sampling.h"


/*
  func process to be called sequently.
 */

inline double biquad::process(double in) {
  /* 
     Transposed Direct Form II is used because it's
     more suitable for float (see article)
   */
  double out = in * b0 + z1;
  z1 = in * b1 + z2 - a1 * out;
  z2 = in * b2 - a2 * out;
  return out;
}

void biquad::apply(amplitude_probes *amp) {
  int i;
  amp->filtered.reserve(amp->count);
  for (i = 0; i < amp->count; i++) {
    amp->filtered.push_back(process((double) (*amp)[i]));
  }
}
  
void biquad::apply(std::vector <double> x) {
  int i;
  for (i = 0; i < x.size(); i++) {
    x[i] = process(x[i]);
  }
}

  
biquad::biquad(std::ifstream& file) {
  z1 = z2 = 0;
  file >> b0 >> b1 >> b2;
  file >> a0 >> a1 >> a2;
}


filter::filter(std::ifstream& file) {
  file >> k;
  cascades.reserve(k);
  for (int i = 0; i < k; i++) {
    cascades.push_back(biquad(file));
  }
}

void filter::apply(amplitude_probes* amp) {
  cascades[0].apply(amp);
  for (int i = 1; i < k; i++) {
    cascades[i].apply(amp->filtered);
  }
}


