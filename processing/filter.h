#include <iostream>
#include <vector>
#include <fstream>
#include "../audio/sampling.h"
#ifndef FILTER_H
#define FILTER_H

class filter {
 private:
  std::vector <double> coef;
 public:
  int M;
  filter (std::ifstream& file);
  filter () {
    M = 1;
    coef.push_back(1);
    coef.push_back(0);
  }; //null filter for debug purposes
  void apply(amplitude_probes* amp);
};

#endif
