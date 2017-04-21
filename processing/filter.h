#include <iostream>
#include <vector>
#include <fstream>
#include "../audio/sampling.h"

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
  }; //null filter
  void apply(amplitude_probes& amp);
};

