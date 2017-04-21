#include <iostream>
#include <vector>
#include <fstream>
#include "filter.h"
#include "../audio/sampling.h"

filter::filter(std::ifstream& file) {
  file >> M;
  coef.reserve(M);
  double temp;
  for (int i = 0; i <= M; i++) {
    file >> temp;
    coef.push_back(temp);
  }
}

void filter::apply(amplitude_probes& amp) {
  /*first samples are not to be processed */
  /* There is a serious problem: it's hard to construct OK
     high pass filter that has f0 = 60 Hz with bitrate freq = 
     44100, 22100, 8000 
  So it's easier to construct low pass filter and substruct filtered noise
  from original signal*/
  int i, j;
  double sum;
  double t;
  amp.filtered.reserve(amp.count);
  for (i=0; i < M; i++) {
    amp.filtered.push_back((double) amp[i]);
  }
  for (i = M; i < amp.count; i++) {
    sum = 0;
    for (j = 0; j <= M; j++) {
      sum += coef[j] * (amp[i-j]);
    }
    t = (double) amp[i] -  sum;
    amp.filtered.push_back(sum);
  }
  return;
}


